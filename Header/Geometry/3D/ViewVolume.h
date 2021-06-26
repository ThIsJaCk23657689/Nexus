#pragma once
#include "Camera.h"
#include "Object.h"

namespace Nexus {
	class ViewVolume : public Object {
	public:

		std::vector<glm::vec4> NearPlaneVertex;
		std::vector<glm::vec4> FarPlaneVertex;
		std::vector<glm::vec3> ViewVolumeNormal;
		std::vector<float> ClippingParameters;
		
		ViewVolume() {
			this->NearPlaneVertex = {
				glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
				glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
				glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),
				glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f),
			};
			
			this->FarPlaneVertex = {
				glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
				glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),
				glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
				glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),
			};
			
			this->ViewVolumeNormal = {
				glm::vec3(0.0f, 0.0f, 1.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, -1.0f),
				glm::vec3(0.0f, -1.0f, 0.0f),
				glm::vec3(-1.0f, 0.0f, 0.0f),
			};
			
			this->ShapeName = "View Volume";
			this->Initialize();
		}

		void UpdateVertices(float camera_fov, bool is_perspective, int width, int height, float global_near, float global_far, float ortho_height, glm::mat4 view) {

			this->NearPlaneVertex.clear();
			this->FarPlaneVertex.clear();
			this->ViewVolumeNormal.clear();
			this->ClippingParameters.clear();
			
			float aspect_wh = width / static_cast<float>(height);
			float aspect_hw = height / static_cast<float>(width);

			glm::vec4 rtnp, ltnp, rbnp, lbnp, rtfp, ltfp, rbfp, lbfp = glm::vec4(1.0f);
			
			if (is_perspective) {
				// 計算透視投影矩陣的各參數
				float p_tn = tan(glm::radians(camera_fov / 2.0f)) * global_near;
				float p_bn = -p_tn;
				float p_rn = p_tn * aspect_wh;
				float p_ln = -p_rn;

				float p_tf = p_tn * global_far / global_near;
				float p_bf = -p_tf;
				float p_rf = p_rn * global_far / global_near;
				float p_lf = -p_rf;

				this->ClippingParameters = {
					p_tn, p_bn, p_ln, p_rn
				};

				// 創建近平面的4個頂點 （記得要將近平面往前多挪0.01，攝影機才不會看不到）
				rtnp = glm::vec4(p_rn, p_tn, -global_near + 0.01, 1.0);
				ltnp = glm::vec4(p_ln, p_tn, -global_near + 0.01, 1.0);
				rbnp = glm::vec4(p_rn, p_bn, -global_near + 0.01, 1.0);
				lbnp = glm::vec4(p_ln, p_bn, -global_near + 0.01, 1.0);

				// 創建遠平面的4個頂點　（記得要將遠平面往後多挪0.01，背景才不會打架）
				rtfp = glm::vec4(p_rf, p_tf, -global_far - 0.01, 1.0);
				ltfp = glm::vec4(p_lf, p_tf, -global_far - 0.01, 1.0);
				rbfp = glm::vec4(p_rf, p_bf, -global_far - 0.01, 1.0);
				lbfp = glm::vec4(p_lf, p_bf, -global_far - 0.01, 1.0);
			} else {
				float r, t = 0.0f;
				r = ortho_height * aspect_wh;
				t = ortho_height;
				
				float l = -r;
				float b = -t;

				this->ClippingParameters = {
					t, b, l, r
				};

				// 創建近平面的4個頂點 （記得要將近平面往前多挪0.01，攝影機才不會看不到）
				rtnp = glm::vec4(r, t, -global_near + 0.01, 1.0);
				ltnp = glm::vec4(l, t, -global_near + 0.01, 1.0);
				rbnp = glm::vec4(r, b, -global_near + 0.01, 1.0);
				lbnp = glm::vec4(l, b, -global_near + 0.01, 1.0);

				// 創建遠平面的4個頂點　（記得要將遠平面往後多挪0.01，背景才不會打架）
				rtfp = glm::vec4(r, t, -global_far - 0.01, 1.0);
				ltfp = glm::vec4(l, t, -global_far - 0.01, 1.0);
				rbfp = glm::vec4(r, b, -global_far - 0.01, 1.0);
				lbfp = glm::vec4(l, b, -global_far - 0.01, 1.0);
			}

			// 取得預設視窗之觀察矩陣後，求反矩陣
			glm::mat4 view_inv = glm::inverse(view);

			// 將這些頂點乘上觀察反矩陣，即可求出世界坐標系頂點（先算近平面）
			rtnp = view_inv * rtnp;
			ltnp = view_inv * ltnp;
			rbnp = view_inv * rbnp;
			lbnp = view_inv * lbnp;

			// 再來換遠平面
			rtfp = view_inv * rtfp;
			ltfp = view_inv * ltfp;
			rbfp = view_inv * rbfp;
			lbfp = view_inv * lbfp;

			this->NearPlaneVertex = {
				rtnp, rbnp, lbnp, ltnp
			};
			this->FarPlaneVertex  = {
				ltfp, lbfp, rbfp, rtfp
			};

			// 清空 View Volume 資料
			std::vector<float>().swap(this->Vertices);
			std::vector<float>().swap(this->Position);
			std::vector<float>().swap(this->Normal);

			glm::vec3 vec_1 = glm::vec3(0.0f);
			glm::vec3 vec_2 = glm::vec3(0.0f);
			glm::vec3 temp_normal = glm::vec3(0.0f);
			
			// 更新 View Volume 的頂點資料
			// ========== Front ==========
			vec_1 = glm::vec3(rtnp - rbnp);
			vec_2 = glm::vec3(lbnp - rbnp);
			temp_normal = glm::normalize(glm::cross(vec_1, vec_2));
			this->ViewVolumeNormal.push_back(temp_normal);
			
			this->AddPosition(lbnp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(ltnp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(rtnp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(rbnp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Top ==========
			vec_1 = glm::vec3(rtnp - rtfp);
			vec_2 = glm::vec3(ltfp - rtfp);
			temp_normal = glm::normalize(glm::cross(vec_2, vec_1));
			this->ViewVolumeNormal.push_back(temp_normal);
			
			this->AddPosition(ltnp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(ltfp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(rtfp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(rtnp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Right ==========
			vec_1 = glm::vec3(rtnp - rtfp);
			vec_2 = glm::vec3(rbfp - rtfp);
			temp_normal = glm::normalize(glm::cross(vec_1, vec_2));
			this->ViewVolumeNormal.push_back(temp_normal);
			
			this->AddPosition(rbnp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(rtnp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(rtfp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(rbfp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Back ==========
			vec_1 = glm::vec3(rtfp - rbfp);
			vec_2 = glm::vec3(lbfp - rbfp);
			temp_normal = glm::normalize(glm::cross(vec_2, vec_1));
			this->ViewVolumeNormal.push_back(temp_normal);
			
			this->AddPosition(rbfp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(rtfp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(ltfp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(lbfp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Bottom ==========
			vec_1 = glm::vec3(rbnp - rbfp);
			vec_2 = glm::vec3(lbfp - rbfp);
			temp_normal = glm::normalize(glm::cross(vec_1, vec_2));
			this->ViewVolumeNormal.push_back(temp_normal);
			
			this->AddPosition(lbfp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(lbnp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(rbnp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(rbfp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Left ==========
			vec_1 = glm::vec3(ltnp - ltfp);
			vec_2 = glm::vec3(lbfp - ltfp);
			temp_normal = glm::normalize(glm::cross(vec_2, vec_1));
			this->ViewVolumeNormal.push_back(temp_normal);
			
			this->AddPosition(lbfp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(ltfp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(ltnp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(lbnp);
			this->AddNormal(temp_normal);
			this->AddTexCoord(1.0f, 0.0f);

			glBindVertexArray(this->VAO->GetID());
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO->GetID());
			glBufferData(GL_ARRAY_BUFFER, this->Vertices.size() * sizeof(float), this->Vertices.data(), GL_STATIC_DRAW);
			glBindVertexArray(0);
		}

		void ShowViewVolumeVerticesImGUI() {
			ImGui::BulletText("rtnp: (%.2f, %.2f, %.2f)", this->NearPlaneVertex[0].x, this->NearPlaneVertex[0].y, this->NearPlaneVertex[0].z);
			ImGui::BulletText("ltnp: (%.2f, %.2f, %.2f)", this->NearPlaneVertex[1].x, this->NearPlaneVertex[1].y, this->NearPlaneVertex[1].z);
			ImGui::BulletText("rbnp: (%.2f, %.2f, %.2f)", this->NearPlaneVertex[2].x, this->NearPlaneVertex[2].y, this->NearPlaneVertex[2].z);
			ImGui::BulletText("lbnp: (%.2f, %.2f, %.2f)", this->NearPlaneVertex[3].x, this->NearPlaneVertex[3].y, this->NearPlaneVertex[3].z);
			ImGui::BulletText("rtfp: (%.2f, %.2f, %.2f)", this->FarPlaneVertex[0].x, this->FarPlaneVertex[0].y, this->FarPlaneVertex[0].z);
			ImGui::BulletText("ltfp: (%.2f, %.2f, %.2f)", this->FarPlaneVertex[1].x, this->FarPlaneVertex[1].y, this->FarPlaneVertex[1].z);
			ImGui::BulletText("rbfp: (%.2f, %.2f, %.2f)", this->FarPlaneVertex[2].x, this->FarPlaneVertex[2].y, this->FarPlaneVertex[2].z);
			ImGui::BulletText("lbfp: (%.2f, %.2f, %.2f)", this->FarPlaneVertex[3].x, this->FarPlaneVertex[3].y, this->FarPlaneVertex[3].z);
		}

		void ShowViewVolumeNormalsImGUI() {
			ImGui::BulletText("Front: (%.2f, %.2f, %.2f)", this->ViewVolumeNormal[0].x, this->ViewVolumeNormal[0].y, this->ViewVolumeNormal[0].z);
			ImGui::BulletText("Top: (%.2f, %.2f, %.2f)", this->ViewVolumeNormal[1].x, this->ViewVolumeNormal[1].y, this->ViewVolumeNormal[1].z);
			ImGui::BulletText("Right: (%.2f, %.2f, %.2f)", this->ViewVolumeNormal[2].x, this->ViewVolumeNormal[2].y, this->ViewVolumeNormal[2].z);
			ImGui::BulletText("Back: (%.2f, %.2f, %.2f)", this->ViewVolumeNormal[3].x, this->ViewVolumeNormal[3].y, this->ViewVolumeNormal[3].z);
			ImGui::BulletText("Bottom: (%.2f, %.2f, %.2f)", this->ViewVolumeNormal[4].x, this->ViewVolumeNormal[4].y, this->ViewVolumeNormal[4].z);
			ImGui::BulletText("Left: (%.2f, %.2f, %.2f)", this->ViewVolumeNormal[5].x, this->ViewVolumeNormal[5].y, this->ViewVolumeNormal[5].z);
		}
	private:

		void GenerateVertices() override {
			// ========== Front ==========
			this->AddPosition(-0.5f, -0.5f, 0.5f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, 0.5f, 0.5f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(0.5f, 0.5f, 0.5f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, 0.5f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Top ==========
			this->AddPosition(-0.5f, 0.5f, 0.5f);
			this->AddNormal(0.0f, 1.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, 0.5f, -0.5f);
			this->AddNormal(0.0f, 1.0f, 0.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(0.5f, 0.5f, -0.5f);
			this->AddNormal(0.0f, 1.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, 0.5f, 0.5f);
			this->AddNormal(0.0f, 1.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Right ==========
			this->AddPosition(0.5f, -0.5f, 0.5f);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(0.5f, 0.5f, 0.5f);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(0.5f, 0.5f, -0.5f);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, -0.5f);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Back ==========
			this->AddPosition(-0.5f, -0.5f, -0.5f);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, 0.5f, -0.5f);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(0.5f, 0.5f, -0.5f);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, -0.5f);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Bottom ==========
			this->AddPosition(-0.5f, -0.5f, 0.5f);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, -0.5f, -0.5f);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, -0.5f);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, 0.5f);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Left ==========
			this->AddPosition(-0.5f, -0.5f, 0.5f);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, 0.5f, 0.5f);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(-0.5f, 0.5f, -0.5f);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(-0.5f, -0.5f, -0.5f);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Indices ==========
			this->AddIndices(0, 1, 2);
			this->AddIndices(0, 2, 3);
			this->AddIndices(4, 5, 6);
			this->AddIndices(4, 6, 7);
			this->AddIndices(8, 9, 10);
			this->AddIndices(8, 10, 11);
			this->AddIndices(12, 13, 14);
			this->AddIndices(12, 14, 15);
			this->AddIndices(16, 17, 18);
			this->AddIndices(16, 18, 19);
			this->AddIndices(20, 21, 22);
			this->AddIndices(20, 22, 23);
		}
	};
}
