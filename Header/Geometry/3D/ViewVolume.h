#pragma once
#include "Camera.h"
#include "Object.h"

namespace Nexus {
	class ViewVolume : public Object {
	public:

		ViewVolume() {
			this->ShapeName = "View Volume";
			this->Initialize();
		}

		void UpdateVertices(float camera_fov, bool is_perspective, int width, int height, float global_left, float global_right, float global_bottom, float global_top, float global_near, float global_far, glm::mat4 view) {

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

				global_left = p_ln;
				global_right = p_rn;
				global_bottom = p_bn;
				global_top = p_tn;

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
				float length = tan(glm::radians(camera_fov / 2)) * global_near * 50;
				float r, t = 0.0f;
				if (width > height) {
					r = length;
					t = length * aspect_hw;
				} else {
					r = length * aspect_wh;
					t = length;
				}
				
				float l = -r;
				float b = -t;

				global_left = l;
				global_right = r;
				global_bottom = b;
				global_top = t;

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

			std::vector<glm::vec4> nearPlaneVertex = {
				rtnp, ltnp, rbnp, lbnp
			};
			std::vector<glm::vec4> farPlaneVertex  = {
				rtfp, ltfp, rbfp, lbfp
			};

			// 清空 View Volume 資料
			std::vector<float>().swap(this->Vertices);
			std::vector<float>().swap(this->Position);
			std::vector<float>().swap(this->Normal);

			// 更新 View Volume 的頂點資料
			// ========== Front ==========
			this->AddPosition(rtnp);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(rbnp);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(lbnp);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(ltnp);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Top ==========
			this->AddPosition(rtnp);
			this->AddNormal(0.0f, 1.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(rtfp);
			this->AddNormal(0.0f, 1.0f, 0.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(ltfp);
			this->AddNormal(0.0f, 1.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(ltnp);
			this->AddNormal(0.0f, 1.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Right ==========
			this->AddPosition(rtnp);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(rtfp);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(rbfp);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(rbnp);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Back ==========
			this->AddPosition(rtfp);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(rbfp);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(lbfp);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(ltfp);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Bottom ==========
			this->AddPosition(rbnp);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(rbfp);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(lbfp);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(lbnp);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Left ==========
			this->AddPosition(ltnp);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(ltfp);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(lbfp);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(lbnp);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			glBindVertexArray(this->VAO->GetID());
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO->GetID());
			glBufferData(GL_ARRAY_BUFFER, this->Vertices.size() * sizeof(float), this->Vertices.data(), GL_STATIC_DRAW);
			glBindVertexArray(0);
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
