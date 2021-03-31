#include "IsoSurface.h"
#include "Logger.h"
#include "FileLoader.h"
#include <iostream>

namespace Nexus {
	IsoSurface::IsoSurface(const std::string& info_path, const std::string& raw_path) {
		this->Initialize(info_path, raw_path);
	}

	void IsoSurface::Initialize(const std::string& info_path, const std::string& raw_path) {
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
		this->ComputeAllNormals();
		this->IsInitialize = true;
	
		Logger::Message(LOG_INFO, "Initialize voxels data completed.");
	}

	std::vector<float> IsoSurface::GetnHistogramData() {
		std::map<unsigned int, unsigned int> histogram;
		for (unsigned int i = 0; i < this->RawData.size(); i++) {
			if (histogram.find(this->RawData[i]) != histogram.end()) {
				histogram[this->RawData[i]]++;
			} else {
				histogram[this->RawData[i]] = 1;
			}
		}

		std::vector<float> x(256, 0.0f);
		for (auto it : histogram ) {
			x[it.first] = static_cast<float>(it.second);
		}
		
		return x;
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
		std::vector<float>().swap(this->Vertices);
		std::vector<float>().swap(this->Position);
		std::vector<float>().swap(this->Normal);

		// Input the data set and iso-value
		this->GenerateVertices(iso_value);

		// Send the position and normal of these vertices to the GPU
		this->BufferInitialize();

		// Ready to draw
		this->IsReadyToDraw = true;

		auto end = std::chrono::system_clock::now();
		this->ElapsedSeconds = end - start;
	}
	
	void IsoSurface::Debug() {
		if (!this->IsInitialize || !this->IsReadyToDraw) {
			Logger::Message(LOG_ERROR, "YOU MUST LOAD THE VOLUME DATA FIRST and COMPUTE THESE ISO SURFACE VERTICES.");
			return;
		}
		
		std::cout << "==================== Iso Surface Information ====================" << std::endl
			<< "Raw File Path: " << this->RawDataFilePath << std::endl
			<< "Construct Method: March Cube Meh" << std::endl
			<< "Voxel Count: " << GetVoxelCount() << std::endl
			<< "Triangle Count: " << GetTriangleCount() << std::endl
			<< "Vertex Count: " << GetVertexCount() << std::endl
			<< "Position Count: " << GetPositionCount() << std::endl
			<< "Normal Count: " << GetNormalCount() << std::endl
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

	void IsoSurface::ComputeAllNormals() {
		// 計算每一個 Voxel 的 Gradient 來當作法向量。
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
					
					this->GridNormals.push_back(norm);
				}
			}
		}
	}
	
	void IsoSurface::GenerateVertices(float iso_value) {

		Logger::Message(LOG_DEBUG, "Starting generate vertices....... It will takes a long time.");
		
		// 開始一個一個 Voxel 讀取，並且每讀一個 Voxel 就抓它其他7個 Voxel (能構成一個正方形的)，
		// 一次輸入 8 個 Voxel，檢查並求出正方塊中所包覆的三角形頂點與法向量為何。
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

		// 先檢查每個方塊上的 Voxel 覆蓋為何，一共有 8 個頂點（使用一個 byte 來代表），如果該 iso value 比較大，就會將該 bits 變成 1。
		for (unsigned int vertex_index = 0; vertex_index < cell.vertices.size(); vertex_index++) {
			if (cell.vertices[vertex_index].Value > iso_value) {
				cube_index |= (1 << vertex_index);
			}
		}

		// 如果都沒有 Voxel 被覆蓋，代表此 Cell 是沒有相交的（可能在圖形 外面 或 裡面）
		if (this->EdgeTable[cube_index] == 0) {
			return;
		}

		// 建立 邊 的順序
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

		// 開始一個一個邊去找有沒有相交，如果有就進行插值計算 相交點的座標以及法向量
		for (unsigned int edge_index = 0; edge_index < EdgeOrder.size(); edge_index++) {
			if (this->EdgeTable[cube_index] & (1 << edge_index)) {
				position_list[edge_index] = this->Interpolation(iso_value, cell.vertices[EdgeOrder[edge_index][0]], cell.vertices[EdgeOrder[edge_index][1]], INTERPOLATE_POSITION);
				normal_list[edge_index] = this->Interpolation(iso_value, cell.vertices[EdgeOrder[edge_index][0]], cell.vertices[EdgeOrder[edge_index][1]], INTERPOLATE_NORMAL);
			}
		}

		// 利用 Lookup table 查表出對應的三角形座標
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
