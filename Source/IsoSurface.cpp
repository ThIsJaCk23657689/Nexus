#include "IsoSurface.h"
#include "Logger.h"
#include "FileLoader.h"
#include "Utill.h"
#include <iostream>

namespace Nexus {
	IsoSurface::IsoSurface(const std::string& info_path, const std::string& raw_path, float max_gradient) {
		this->Initialize(info_path, raw_path, max_gradient);
	}

	void IsoSurface::Initialize(const std::string& info_path, const std::string& raw_path, float max_gradient) {
		Logger::Message(LOG_INFO, "Starting loading volume data: " + raw_path);
		this->IsInitialize = false;
		Settings.Resolution[0] = 149;
		Settings.Resolution[1] = 208;
		Settings.Resolution[2] = 110;
		Settings.DataType = "unsigned char";

		this->RawDataFilePath = raw_path;
		this->InfDataFilePath = info_path;

		// Loading Volume Data
		this->RawData = Nexus::FileLoader::LoadRawFile(raw_path);
		Logger::Message(LOG_INFO, "Starting initialize voxels data...");
		
		// Compute the gradient of these all voxels.
		this->GradientMagnitudes.clear();
		this->ComputeAllNormals(max_gradient);

		// �p�� Iso value Histogram�BGradient Histogram �M heatmap
		this->GenerateIsoValueHistogram();
		this->GenerateGradientHistogram();
		this->GenerateGradientHeatMap();

		this->IsInitialize = true;

		// ���ެO 0 ~ 3409119 �ӡA�N��C�@�� voxel �W�� gradient �����סA���� 1 �� max_gradient �����C
		// Utill::Show1DVectorStatistics(this->GradientMagnitudes, "Gradient Before");
	
		Logger::Message(LOG_INFO, "Initialize voxels data completed.");
	}

	void IsoSurface::ConvertToPolygon(float iso_value) {
		if (!this->IsInitialize) {
			Logger::Message(LOG_ERROR, "YOU MUST LOAD THE VOLUME DATA FIRST!");
			return;
		}

		// Get the start time.
		auto start = std::chrono::system_clock::now();

		// Initialize and clean the vector;
		this->IsReadyToDraw = false;
		this->Vertices.clear();
		this->Position.clear();
		this->Normal.clear();

		// Input the data set and iso-value
		this->GenerateVertices(iso_value);

		// Send the position and normal of these vertices to the GPU
		this->BufferInitialize();

		// Ready to draw
		this->IsReadyToDraw = true;

		auto end = std::chrono::system_clock::now();
		this->ElapsedSeconds = end - start;
	}

	void IsoSurface::GenerateIsoValueHistogram() {
		// ��l�ơA�N�� Volume Data ����Ƥ��� m ����
		this->IsoValueHistogram = std::vector<float>(this->Interval, 0.0f);

		// ��������X��Ƥ��̤j�Ȱ����U�ɡA�W�ɫh�ĥ� 0�C
		float max_isovalue = *std::max_element(this->RawData.cbegin(), this->RawData.cend());

		// �����N iso value ���� 0 �� max_isovalue ���� ���� m �ӵ����A���D���Z
		float bin_width = ((max_isovalue - 0) + 1) / this->Interval;
		this->IsoValue_Boundary.clear();
		for (unsigned int i = 0; i < this->Interval; i++) {
			float up = bin_width * i;
			float down = bin_width * (i + 1);
			this->IsoValue_Boundary.push_back(std::pair<float, float>(up, down));
		}

		// ���y��Ӹ�ơA�ھڶ��Z�h���P�_
		for (unsigned int i = 0; i < this->RawData.size(); i++) {
			for (unsigned int j = 0; j < this->IsoValue_Boundary.size(); j++) {
				if (this->IsoValue_Boundary[j].first <= static_cast<int>(this->RawData[i]) && static_cast<int>(this->RawData[i]) < this->IsoValue_Boundary[j].second) {
					this->IsoValueHistogram[j] += 1;
				}
			}
		}

		// ��ܲέp��T
		// Utill::Show1DVectorStatistics(x, "Iso Value Histogram");
	}
	
	void IsoSurface::GenerateGradientHistogram() {
		// ��l�ơA�N gradient length (�w�g�Q������) ����Ƥ��� k ����
		this->GradientHistogram = std::vector<float>(this->Interval, 0.0f);

		// ��������X��Ƥ��̤j�Ȱ����U�ɡA�W�ɫh�ĥ� 0�C
		float max_gradient = *std::max_element(this->GradientMagnitudes.cbegin(), this->GradientMagnitudes.cend());

		// �����N gradient �����פ��� 0 �� max_gradient ���� ���� k(16) �ӵ����A���D���Z
		float bin_width = ((max_gradient - 0) + 1) / this->Interval;
		this->Gradient_Boundary.clear();
		for (unsigned int i = 0; i < this->Interval; i++) {
			float up = bin_width * i;
			float down = bin_width * (i + 1);
			this->Gradient_Boundary.push_back(std::pair<float, float>(up, down));
		}

		// ���y��Ӹ�ơA�ھڶ��Z�h���P�_
		for (unsigned int i = 0; i < this->GradientMagnitudes.size(); i++) {
			for (unsigned int j = 0; j < this->Gradient_Boundary.size(); j++) {
				if (this->Gradient_Boundary[j].first <= this->GradientMagnitudes[i] && this->GradientMagnitudes[i] < this->Gradient_Boundary[j].second) {
					this->GradientHistogram[j] += 1;
				}
			}
		}

		// ��ܲέp��T
		// Utill::Show1DVectorStatistics(this->GradientMagnitudes, "Gradient Histogram");
	}
	
	void IsoSurface::GenerateGradientHeatMap() {
		// ��l�ơA��b�� Iso Value�A�a�b�� Gradient Length
		this->GradientHeatmap = std::vector<float>(this->Interval * this->Interval, 0.0f);

		// �]�M�Ҧ���ơA�u�n�� Voxel �ŦX������ Iso value �M �b�۹����� ��װ϶� �Ӯ�N +=1
		for (unsigned int i = 0; i < this->RawData.size(); i++) {
			unsigned int isovalue_idx = 0;
			unsigned int gradient_idx = 0;
			for (unsigned int j = 0; j < this->Gradient_Boundary.size(); j++) {
				gradient_idx = j;
				if (this->Gradient_Boundary[j].first <= this->GradientMagnitudes[i] && this->GradientMagnitudes[i] < this->Gradient_Boundary[j].second) {
					break;
				}
			}

			for (unsigned int j = 0; j < this->IsoValue_Boundary.size(); j++) {
				isovalue_idx = j;
				if (this->IsoValue_Boundary[j].first <= static_cast<int>(this->RawData[i]) && static_cast<int>(this->RawData[i]) < this->IsoValue_Boundary[j].second) {
					break;
				}
			}
			int idx = ((this->Interval - 1) - gradient_idx) * this->Interval + isovalue_idx;
			this->GradientHeatmap[idx] += 1;
		}
	}
	
	void IsoSurface::IsoValueHistogramEqualization() {
		// �����o�ª� histogram�A�í��s��l��
		std::vector<float> old_histogram = this->IsoValueHistogram;
		this->IsoValueHistogram = std::vector<float>(256, 0.0f);

		// �p���`�Ҧ� iso value �� 0 ~ 255 ����ܦ��ơC 
		float total_iso_value = std::accumulate(old_histogram.cbegin(), old_histogram.cend(), 0.0);

		// �k�@�ơA�Ҧ���ܦ��ư��H�`���ơA�ƭȷ|���� 0 ~ 1 �����C
		std::vector<float> normalize;
		for (auto i : old_histogram) {
			normalize.push_back(i / total_iso_value);
		}

		// �p��	�֭p�����
		float current_value = 0;
		std::vector<float> cumulative;
		for (auto i : normalize) {
			current_value += i;
			cumulative.push_back(current_value);
		}

		// �A���s�M�g�� 0 ~ 255 (�åB�|�ˤ��J)
		std::vector<int> equal_values;
		for (auto i : cumulative) {
			equal_values.push_back(round(i * 255));
		}

		// �o�쪺�ƭȬO�ҿת����Ťƭ� ��p equal_values[0] = 71 �N�� �쥻  iso value = 0 ���Ťƫ�A�վ㬰iso value = 71  �ƶq�O���ܪ�
		for (int i = 0; i < equal_values.size(); i++) {
			this->IsoValueHistogram[equal_values[i]] += old_histogram[i];
		}

		// �̫�O�ѤF�n���� Volume Data ���@�˪��ާ@�]���Ȥơ^�I
		this->EqualizationData(equal_values);
	}

	std::vector<float> IsoSurface::GetIsoValueHistogram() {
		return this->IsoValueHistogram;
	}
	
	std::vector<float> IsoSurface::GetGradientHistogram() {
		return this->GradientHistogram;
	}
	
	std::vector<float> IsoSurface::GetGradientHeatmap() {
		return this->GradientHeatmap;
	}

	std::vector<char*> IsoSurface::GetGradientHeatmapAxisLabels(bool is_axis_x) {
		std::vector<char*> result;
		if (is_axis_x) {
			float x_bin = this->IsoValue_Boundary.size() / 4.0f;
			for (unsigned int i = 0; i <= 4; i++) {
				std::string temp;
				if (i == 4) {
					temp = std::to_string(static_cast<int>(this->IsoValue_Boundary[(x_bin * i) - 1].second));
				} else {
					temp = std::to_string(static_cast<int>(this->IsoValue_Boundary[(x_bin * i)].first));
				}
				std::cout << temp.c_str() << std::endl;
				result.push_back(const_cast<char*>(temp.c_str()));
			}
		} else {
			float y_bin = this->Gradient_Boundary.size() / 4.0f;
			for (unsigned int i = 0; i <= 4; i++) {
				std::string temp;
				if (i == 4) {
					temp = std::to_string(static_cast<int>(this->Gradient_Boundary[(y_bin * i) - 1].second));
				} else {
					temp = std::to_string(static_cast<int>(this->Gradient_Boundary[(y_bin * i)].first));
				}
				std::cout << temp.c_str() << std::endl;
				result.push_back(const_cast<char*>(temp.c_str()));
			}
		}
		return result;
	}

	void IsoSurface::EqualizationData(std::vector<int> equal_values) {
		for (unsigned int i = 0; i < this->RawData.size(); i++) {
			float after_value = equal_values[static_cast<int>(this->RawData[i])];
			this->RawData[i] = after_value;
		}
	}

	void IsoSurface::Debug() {
		if (!this->IsInitialize || !this->IsReadyToDraw) {
			Logger::Message(LOG_ERROR, "YOU MUST LOAD THE VOLUME DATA FIRST and COMPUTE THESE ISO SURFACE VERTICES.");
			return;
		}
		
		std::cout << "==================== Iso Surface Information ====================" << std::endl
			<< "Raw File Path: " << this->RawDataFilePath << std::endl
			<< "Construct Method: March Cube Method" << std::endl
			<< "Voxel Count: " << GetVoxelCount() << std::endl
			<< "Triangle Count: " << GetTriangleCount() << std::endl
			<< "Vertex Count: " << GetVertexCount() << std::endl
			<< "Position Count: " << GetPositionCount() << std::endl
			<< "Normal Count: " << GetNormalCount() << std::endl
			<< "Gradient Magnitudes Counts: " << this->GradientMagnitudes.size() << std::endl
			<< "Elapsed time: " << this->ElapsedSeconds.count() << " (seconds)" << std::endl
			<< "================================================================================" << std::endl;
	}

	void IsoSurface::Draw(Nexus::Shader* shader, glm::mat4 model) {
		if (!this->IsInitialize || !this->IsReadyToDraw) {
			Logger::Message(LOG_ERROR, "YOU MUST LOAD THE VOLUME DATA FIRST and COMPUTE THESE ISO SURFACE VERTICES.");
			return;
		}
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		shader->Use();
		shader->SetMat4("model", model);

		// this->VAO->Bind();
		glBindVertexArray(VAO);
		if (this->EnableWireFrameMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glDrawArrays(GL_TRIANGLES, 0, this->GetPositionCount());
		glBindVertexArray(0);
	}

	void IsoSurface::ComputeAllNormals(float max_gradient) {
		// �p��C�@�� Voxel �� Gradient �ӷ�@�k�V�q�C
		for (int k = 0; k < Settings.Resolution[2]; k++) {
			for (int j = 0; j < Settings.Resolution[1]; j++) {
				for (int i = 0; i < Settings.Resolution[0]; i++) {
					glm::vec3 norm = glm::vec3(0.0f);

					if (i + 1 >= Settings.Resolution[0]) {
						// Backward difference
						norm.x = (this->GetIsoValueFromGrid(i, j, k) - this->GetIsoValueFromGrid(i - 1, j, k)) / Settings.Ratio.x;
					} else if (i - 1 < 0) {
						// Forward difference
						norm.x = (this->GetIsoValueFromGrid(i + 1, j, k) - this->GetIsoValueFromGrid(i, j, k)) / Settings.Ratio.x;
					} else {
						// Central difference
						norm.x = (this->GetIsoValueFromGrid(i + 1, j, k) - this->GetIsoValueFromGrid(i - 1, j, k)) / 2 * Settings.Ratio.x;
					}

					if (j + 1 >= Settings.Resolution[1]) {
						// Backward difference
						norm.y = (this->GetIsoValueFromGrid(i, j, k) - this->GetIsoValueFromGrid(i, j - 1, k)) / Settings.Ratio.y;
					} else if (j - 1 < 0) {
						// Forward difference
						norm.y = (this->GetIsoValueFromGrid(i, j + 1, k) - this->GetIsoValueFromGrid(i, j, k)) / Settings.Ratio.y;
					} else {
						// Central difference
						norm.y = (this->GetIsoValueFromGrid(i, j + 1, k) - this->GetIsoValueFromGrid(i, j - 1, k)) / 2 * Settings.Ratio.y;
					}

					if (k + 1 >= Settings.Resolution[2]) {
						// Backward difference
						norm.z = (this->GetIsoValueFromGrid(i, j, k) - this->GetIsoValueFromGrid(i, j, k - 1)) / Settings.Ratio.z;
					} else if (k - 1 < 0) {
						// Forward difference
						norm.z = (this->GetIsoValueFromGrid(i, j, k + 1) - this->GetIsoValueFromGrid(i, j, k)) / Settings.Ratio.z;
					} else {
						// Central difference
						norm.z = (this->GetIsoValueFromGrid(i, j, k + 1) - this->GetIsoValueFromGrid(i, j, k - 1)) / 2 * Settings.Ratio.z;
					}

					// Gradient ���פ�����
					float temp_length = glm::length(norm);
					if (temp_length < 1) {
						temp_length = 1;
					} else if (temp_length > max_gradient) {
						temp_length = max_gradient;
					}
					temp_length = 20.0f * glm::log2(temp_length);
					this->GradientMagnitudes.push_back(temp_length);
					
					this->GridNormals.push_back(norm);
				}
			}
		}
	}
	
	void IsoSurface::GenerateVertices(float iso_value) {

		Logger::Message(LOG_DEBUG, "Starting generate vertices....... It will takes a long time.");
		
		// �}�l�@�Ӥ@�� Voxel Ū���A�åB�CŪ�@�� Voxel �N�쥦��L7�� Voxel (��c���@�ӥ���Ϊ�)�A
		// �@����J 8 �� Voxel�A�ˬd�èD�X��������ҥ]�Ъ��T���γ��I�P�k�V�q����C
		for (int k = 0; k < Settings.Resolution[2] - 1; k++) {
			for (int j = 0; j < Settings.Resolution[1] - 1; j++) {
				for (int i = 0; i < Settings.Resolution[0] - 1; i++) {
					// Logger::Message(LOG_DEBUG, "Coordinate: (" + std::to_string(i) + ", " + std::to_string(j) + ", " + std::to_string(k) + ")");
					std::vector<glm::vec3> VertexOrder = {
						glm::vec3(i, j, k),
						glm::vec3(i + 1, j, k),
						glm::vec3(i + 1, j, k + 1),
						glm::vec3(i, j, k + 1),
						glm::vec3(i, j + 1, k),
						glm::vec3(i + 1, j + 1, k),
						glm::vec3(i + 1, j + 1, k + 1),
						glm::vec3(i, j + 1, k + 1),
					};

					auto cell = GridCell();
					for(int vertex_index = 0; vertex_index < VertexOrder.size(); vertex_index++) {
						auto voxel = Voxel();
						voxel.Position = VertexOrder[vertex_index];
						voxel.Normal = this->GetNormalFromGrid(VertexOrder[vertex_index]);
						voxel.Value = this->GetIsoValueFromGrid(VertexOrder[vertex_index]);
						cell.vertices.push_back(voxel);
					}

					Polygonise(cell, iso_value);
				}
			}
		}
		Logger::Message(LOG_DEBUG, "Generate vertices completed.");
	}

	void IsoSurface::Polygonise(GridCell cell, float iso_value) {

		int cube_index = 0;
		glm::vec3 position_list[12];
		glm::vec3 normal_list[12];

		// ���ˬd�C�Ӥ���W�� Voxel �л\����A�@�@�� 8 �ӳ��I�]�ϥΤ@�� byte �ӥN��^�A�p�G�� iso value ����j�A�N�|�N�� bits �ܦ� 1�C
		for (unsigned int vertex_index = 0; vertex_index < cell.vertices.size(); vertex_index++) {
			if (cell.vertices[vertex_index].Value > iso_value) {
				cube_index |= (1 << vertex_index);
			}
		}

		// �p�G���S�� Voxel �Q�л\�A�N�� Cell �O�S���ۥ檺�]�i��b�ϧ� �~�� �� �̭��^
		if (this->EdgeTable[cube_index] == 0) {
			return;
		}

		// �إ� �� ������
		std::vector<std::vector<int>> EdgeOrder = {
			std::vector<int>{0, 1},
			std::vector<int>{1, 2},
			std::vector<int>{2, 3},
			std::vector<int>{3, 0},
			std::vector<int>{4, 5},
			std::vector<int>{5, 6},
			std::vector<int>{6, 7},
			std::vector<int>{7, 4},
			std::vector<int>{0, 4},
			std::vector<int>{1, 5},
			std::vector<int>{2, 6},
			std::vector<int>{3, 7},
		};

		// �}�l�@�Ӥ@����h�䦳�S���ۥ�A�p�G���N�i�洡�ȭp�� �ۥ��I���y�ХH�Ϊk�V�q
		for (unsigned int edge_index = 0; edge_index < EdgeOrder.size(); edge_index++) {
			if (this->EdgeTable[cube_index] & (1 << edge_index)) {
				position_list[edge_index] = this->Interpolation(iso_value, cell.vertices[EdgeOrder[edge_index][0]], cell.vertices[EdgeOrder[edge_index][1]], INTERPOLATE_POSITION);
				normal_list[edge_index] = this->Interpolation(iso_value, cell.vertices[EdgeOrder[edge_index][0]], cell.vertices[EdgeOrder[edge_index][1]], INTERPOLATE_NORMAL);
			}
		}

		// �Q�� Lookup table �d��X�������T���ήy��
		for (unsigned int i = 0; this->TriangleTable[cube_index][i] != -1; i += 3) {
			for (unsigned int offset = 0; offset < 3; offset++) {
				this->AddPosition(position_list[this->TriangleTable[cube_index][i + offset]]);
				this->AddNormal(glm::normalize(normal_list[this->TriangleTable[cube_index][i + offset]]));
			}
		}
	}

	glm::vec3 IsoSurface::Interpolation(float iso_value, Voxel voxel_a, Voxel voxel_b, InterpolateMode mode) {

		glm::vec3 p1, p2;
		if (mode == INTERPOLATE_POSITION) {
			p1 = voxel_a.Position;
			p2 = voxel_b.Position;
		} else if (mode == INTERPOLATE_NORMAL) {
			p1 = voxel_a.Normal;
			p2 = voxel_b.Normal;
		} else {
			assert(false);
		}
		
		float val1 = voxel_a.Value;
		float val2 = voxel_b.Value;
		float proportion;
		glm::vec3 result;

		if (abs(iso_value - val1) < 0.00001) {
			return p1;
		}

		if (abs(iso_value - val2) < 0.00001) {
			return p2;
		}

		if (abs(val1 - val2) < 0.00001) {
			return p1;
		}

		proportion = (iso_value - val1) / (val2 - val1);
		result = p1 + (float)proportion * (p2 - p1);

		return result;
	}
	
	void IsoSurface::BufferInitialize() {
		/*
		Nexus::VertexAttributes Attribs[] = { {3, 0}, {3, offsetof(Vertex, Normal)} };
		this->VBO = std::make_unique<Nexus::VertexBuffer>(this->Vertices.data(), this->Vertices.size() * sizeof(float));
		this->VAO = std::make_unique<Nexus::VertexArray>(this->VBO.get(), Attribs, 2, (GLsizei)sizeof(float));
		*/
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, this->Vertices.size() * sizeof(float), this->Vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}
	
	void IsoSurface::AddPosition(float x, float y, float z) {
		this->Position.push_back(x);
		this->Position.push_back(y);
		this->Position.push_back(z);

		this->Vertices.push_back(x);
		this->Vertices.push_back(y);
		this->Vertices.push_back(z);
	}

	void IsoSurface::AddPosition(glm::vec3 position) {
		this->Position.push_back(position.x);
		this->Position.push_back(position.y);
		this->Position.push_back(position.z);

		this->Vertices.push_back(position.x);
		this->Vertices.push_back(position.y);
		this->Vertices.push_back(position.z);
	}

	void IsoSurface::AddNormal(float nx, float ny, float nz) {
		this->Normal.push_back(nx);
		this->Normal.push_back(ny);
		this->Normal.push_back(nz);

		this->Vertices.push_back(nx);
		this->Vertices.push_back(ny);
		this->Vertices.push_back(nz);
	}
	
	void IsoSurface::AddNormal(glm::vec3 normal) {
		this->Normal.push_back(normal.x);
		this->Normal.push_back(normal.y);
		this->Normal.push_back(normal.z);

		this->Vertices.push_back(normal.x);
		this->Vertices.push_back(normal.y);
		this->Vertices.push_back(normal.z);
	}
}
