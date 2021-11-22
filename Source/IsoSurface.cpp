#include "IsoSurface.h"
#include "Logger.h"
#include "FileLoader.h"
#include "Utill.h"
#include "Cube.h"
#include <iostream>

namespace Nexus {
	IsoSurface::IsoSurface(const std::string& info_path, const std::string& raw_path, float max_gradient) {
		this->Initialize(info_path, raw_path, max_gradient);
	}

	void IsoSurface::Initialize(const std::string& info_path, const std::string& raw_path, float max_gradient) {
		Logger::Message(LOG_INFO, "Starting loading volume data: " + raw_path);

		// Initial
		this->IsInitialize = false;
		this->IsReadyToDraw = false;
		this->IsEqualization = false;
		this->InfData.clear();
		this->RawData.clear();
		this->GridNormals.clear();
		this->GradientMagnitudes.clear();
		this->TextureData.clear();
		
		this->RawDataFilePath = raw_path;
		this->InfDataFilePath = info_path;

		// Loading Info File
		this->InfData = Nexus::FileLoader::LoadInfoFile(info_path);
		this->GetAttributesFromInfoFile();
		
		// Loading Volume Data
        Nexus::FileLoader::LoadRawFile(this->RawData, raw_path, Attributes);
		Logger::Message(LOG_INFO, "Starting initialize voxels data...");

		// Compute the gradient of these all voxels.
		this->ComputeAllNormals(max_gradient);

		// 計算 Iso value Histogram、Gradient Histogram 和 heatmap
		this->GenerateIsoValueHistogram();
		this->GenerateGradientHistogram();
		this->GenerateGradientHeatMap();
		
		// 索引是 0 ~ 3409119 個，代表每一個 voxel 上面 gradient 的長度，介於 1 到 max_gradient 之間。
		// Utill::Show1DVectorStatistics(this->GradientMagnitudes, "Gradient Histogram - Before");

		this->IsInitialize = true;
		Logger::Message(LOG_INFO, "Initialize voxels data completed.");
	}

	void IsoSurface::GetAttributesFromInfoFile() {
		glm::vec3 resolution(0.0f);
		glm::vec3 voxelsize(0.0f);
		std::string sampletype;
		std::string endian;

		std::stringstream info_ss = std::stringstream{ this->InfData };
		std::string line;
		
		std::vector<std::regex> RegFormat = {
			std::regex("[Rr]esolution"),
			std::regex("[Vv]oxel[-_ ]?[Ss]ize|[Rr]atio"),
			std::regex("[Ss]ample[-_ ]?[Tt]ype"),
			std::regex("[Ee]ndian")
		};

		while (std::getline(info_ss, line)) {
			std::vector<bool> result;
			for (auto i : RegFormat) {
				result.push_back(std::regex_search(line, i));
			}

			std::cout << line << "\t result: ";
			for (auto i : result) {
				std::cout << std::to_string(i);
			}

            if (std::regex_search(line, std::regex("^#"))) {
                // 如果第一行有 # 就視為註解
                std::cout << "comment" << std::endl;
                continue;
            }

			if (std::regex_search(line, RegFormat[0])) {
				// 如果是 Resolution，格式通常是 Resolution=149:208:110、resolution=256x256x256。
				// 先找出 = 的位置後，擷取剩下的字串，並利用:來進行切割(或x來切割)。
				std::vector<std::string> resolution_str;
				if(line.find("x") == std::string::npos) {
					// 如果找不到 x 就用 : 去切割
					resolution_str = Utill::Split(line.substr(line.find("=") + 1), ":");
				} else {
					resolution_str = Utill::Split(line.substr(line.find("=") + 1), "x");
				}
				
				resolution.x = std::stof(resolution_str[0]);
				resolution.y = std::stof(resolution_str[1]);
				resolution.z = std::stof(resolution_str[2]);

				Attributes.Resolution.x = std::stof(resolution_str[0]);
				Attributes.Resolution.y = std::stof(resolution_str[1]);
				Attributes.Resolution.z = std::stof(resolution_str[2]);
				
				std::cout << resolution.x << " ";
				std::cout << resolution.y << " ";
				std::cout << resolution.z << " ";

                std::cout << std::endl;
                continue;
			}

			if (std::regex_search(line, RegFormat[1])) {
				// 如果是 VoxelSize，格式通常是 VoxelSize=1.000000:1.000000:1.000000、ratio=1:0.5:1。
				// 先找出 = 的位置後，擷取剩下的字串，並利用:來進行切割。
				std::vector<std::string> voxelsize_str = Utill::Split(line.substr(line.find("=") + 1), ":");

				voxelsize.x = std::stof(voxelsize_str[0]);
				voxelsize.y = std::stof(voxelsize_str[1]);
				voxelsize.z = std::stof(voxelsize_str[2]);

				Attributes.Ratio.x = std::stof(voxelsize_str[0]);
				Attributes.Ratio.y = std::stof(voxelsize_str[1]);
				Attributes.Ratio.z = std::stof(voxelsize_str[2]);

				std::cout << voxelsize.x << " ";
				std::cout << voxelsize.y << " ";
				std::cout << voxelsize.z << " ";

                std::cout << std::endl;
                continue;
			}

			if (std::regex_search(line, RegFormat[2])) {
				// 如果是 SampleType，格式通常是 SampleType=UnsignedChar、sample-type=unsigned char。
				// 先找出 = 的位置後，擷取剩下的字串。
				std::cout << line.find("=") << "\t" << line.substr(line.find("=") + 1) << "\t";
				std::string sampletype_str = line.substr(line.find("=") + 1);
				if (std::regex_search(line, std::regex("[Cc]har"))) {
                    Attributes.DataType = VolumeDataType_Char;
                } else if (std::regex_search(line, std::regex("[Ss]hort"))) {
                    Attributes.DataType = VolumeDataType_Short;
                } else if (std::regex_search(line, std::regex("[Ii]nt[eger]?"))) {
                    Attributes.DataType = VolumeDataType_Int;
                } else if (std::regex_search(line, std::regex("[Ll]ong"))) {
                    Attributes.DataType = VolumeDataType_Long;
                } else if (std::regex_search(line, std::regex("[Uu]nsigned[-_ ]?[Cc]har"))) {
                    Attributes.DataType = VolumeDataType_UnsignedChar;
                } else if (std::regex_search(line, std::regex("[Uu]nsigned[-_ ]?[Ss]hort"))) {
                    Attributes.DataType = VolumeDataType_UnsignedShort;
                } else if (std::regex_search(line, std::regex("[Uu]nsigned[-_ ]?[Ii]nt[eger]?"))) {
                    Attributes.DataType = VolumeDataType_UnsignedInt;
                } else if (std::regex_search(line, std::regex("[Uu]nsigned[-_ ]?[Ll]ong"))) {
                    Attributes.DataType = VolumeDataType_UnsignedLong;
                }

                std::cout << std::endl;
                continue;
			}

			if (std::regex_search(line, RegFormat[3])) {
				// 如果是 Endian，格式通常是 Endian=Little。
				// 先找出 = 的位置後，擷取剩下的字串。
				std::cout << line.find("=") << "\t" << line.substr(line.find("=") + 1) << "\t";
				std::string endian_str = line.substr(line.find("=") + 1);
				if (std::regex_search(line, std::regex("[Ll]ittle")) || endian_str == "") {
                    Attributes.Endian = "little";
                } else if (std::regex_search(line, std::regex("[B]ig"))) {
                    Attributes.Endian = "big";
                }

                std::cout << std::endl;
                continue;
			}

			std::cout << std::endl;
		}
	}

	void IsoSurface::GenerateTextureData() {
		float max_isovalue = *std::max_element(this->RawData.cbegin(), this->RawData.cend());
		
		for (unsigned i = 0; i < this->RawData.size(); i++) {
			glm::vec3 temp_norm = this->GridNormals[i];
			float temp_value = this->RawData[i] / max_isovalue;
			this->TextureData.push_back(glm::vec4(temp_norm, temp_value));
		}
	}
	
	void IsoSurface::ConvertToPolygon() {
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

		if (this->CurrentRenderMode == RENDER_MODE_ISO_SURFACE) {

			// Input the data set and iso-value
			this->GenerateVertices(this->IsoValue);

			// Send the position and normal of these vertices to the GPU
			this->BufferInitialize();
			
		} else if (this->CurrentRenderMode == RENDER_MODE_RAY_CASTING) {
			// Generate a new data and sent into gpu (r, g, b) => Gradient, a => Value;
			this->GenerateTextureData();

			// Creating a 3D Texture.
			// Please make sure your texture settings (it is GL_FLOAT, not GL_UNSIGNED_BYTE)
			glGenTextures(1, &this->VolumeTexture);
			glBindTexture(GL_TEXTURE_3D, this->VolumeTexture);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, static_cast<GLsizei>(Attributes.Resolution.x), static_cast<GLsizei>(Attributes.Resolution.y), static_cast<GLsizei>(Attributes.Resolution.z), 0, GL_RGBA, GL_FLOAT, this->TextureData.data());
			glBindTexture(GL_TEXTURE_3D, 0);

			// Creating a bounding-box with texture coordinate.
			glm::vec3 resolution = Attributes.Resolution * Attributes.Ratio;
			this->BoundingBoxVertices = {
				resolution.x, resolution.y, 0.0,				1.0, 1.0, 0.0,
				resolution.x, 0.0, 0.0,							1.0, 0.0, 0.0,
				0.0, 0.0, 0.0,									0.0, 0.0, 0.0,
				0.0, resolution.y, 0.0,							0.0, 1.0, 0.0,
				resolution.x,  resolution.y, resolution.z,		1.0, 1.0, 1.0,
				resolution.x, 0.0, resolution.z,				1.0, 0.0, 1.0,
				0.0, 0.0, resolution.z,							0.0, 0.0, 1.0,
				0.0,  resolution.y, resolution.z,				0.0, 1.0, 1.0,
			};
			this->BoundingBoxIndices = {
				0, 1, 2,
				0, 2, 3,
				3, 2, 6,
				3, 6, 7,
				7, 6, 5,
				7, 5, 4,
				4, 5, 1,
				4, 1, 0,
				4, 0, 3,
				4, 3, 7,
				6, 2, 1,
				6, 1, 5
			};
			glGenVertexArrays(1, &BoundingBoxVAO);
			glGenBuffers(1, &BoundingBoxVBO);
			glGenBuffers(1, &BoundingBoxEBO);
			glBindVertexArray(BoundingBoxVAO);
			glBindBuffer(GL_ARRAY_BUFFER, BoundingBoxVBO);
			glBufferData(GL_ARRAY_BUFFER, this->BoundingBoxVertices.size() * sizeof(float), this->BoundingBoxVertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BoundingBoxEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->BoundingBoxIndices.size() * sizeof(unsigned int), this->BoundingBoxIndices.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)(3 * sizeof(float)));
			glBindVertexArray(0);
		}

		// Ready to draw
		this->IsReadyToDraw = true;

		auto end = std::chrono::system_clock::now();
		this->ElapsedSeconds = end - start;
	}

	void IsoSurface::GenerateIsoValueHistogram() {
		// 初始化，將此 Volume Data 的資料分成 m 等份
		this->IsoValueHistogram = std::vector<float>(static_cast<unsigned int>(this->Interval), 0.0f);

		// 必須先找出資料中最大值做為下界，上界則採用 0。
		float max_isovalue = *std::max_element(this->RawData.cbegin(), this->RawData.cend());

		// 必須將 iso value 介於 0 到 max_isovalue 之間 切成 m 個等分，先求間距
		float bin_width = ((max_isovalue - 0) + 1) / this->Interval;
		this->IsoValueBoundary.clear();
		for (unsigned int i = 0; i < this->Interval; i++) {
			float up = bin_width * i;
			float down = bin_width * (i + 1);
			this->IsoValueBoundary.push_back(std::pair<float, float>(up, down));
		}

		// 掃描整個資料，根據間距去做判斷
		for (unsigned int i = 0; i < this->RawData.size(); i++) {
			for (unsigned int j = 0; j < this->IsoValueBoundary.size(); j++) {
				if (this->IsoValueBoundary[j].first <= static_cast<int>(this->RawData[i]) && static_cast<int>(this->RawData[i]) < this->IsoValueBoundary[j].second) {
					this->IsoValueHistogram[j] += 1;
				}
			}
		}

		// 顯示統計資訊
		// Utill::Show1DVectorStatistics(x, "Iso Value Histogram");
	}
	
	void IsoSurface::GenerateGradientHistogram() {
		// 初始化，將 gradient length (已經被分貝化) 的資料分成 k 等份
		this->GradientHistogram = std::vector<float>(static_cast<unsigned int>(this->Interval), 0.0f);

		// 必須先找出資料中最大值做為下界，上界則採用 0。
		float max_gradient = *std::max_element(this->GradientMagnitudes.cbegin(), this->GradientMagnitudes.cend());

		// 必須將 gradient 的長度介於 0 到 max_gradient 之間 切成 k 個等分，先求間距
		float bin_width = ((max_gradient - 0) + 1) / this->Interval;
		this->GradientBoundary.clear();
		for (unsigned int i = 0; i < this->Interval; i++) {
			float up = bin_width * i;
			float down = bin_width * (i + 1);
			this->GradientBoundary.push_back(std::pair<float, float>(up, down));
		}

		// 掃描整個資料，根據間距去做判斷
		for (unsigned int i = 0; i < this->GradientMagnitudes.size(); i++) {
			for (unsigned int j = 0; j < this->GradientBoundary.size(); j++) {
				if (this->GradientBoundary[j].first <= this->GradientMagnitudes[i] && this->GradientMagnitudes[i] < this->GradientBoundary[j].second) {
					this->GradientHistogram[j] += 1;
				}
			}
		}

		// 顯示統計資訊
		// Utill::Show1DVectorStatistics(this->GradientMagnitudes, "Gradient Histogram - After");
	}
	
	void IsoSurface::GenerateGradientHeatMap() {
		// 初始化，橫軸為 Iso Value，縱軸為 Gradient Length
		this->GradientHeatmap = std::vector<float>(static_cast<unsigned int>(this->Interval * this->Interval), 0.0f);

		// 跑遍所有資料，只要有 Voxel 符合對應的 Iso value 和 在相對應的 梯度區間 該格就 +=1
		for (unsigned int i = 0; i < this->RawData.size(); i++) {
			unsigned int isovalue_idx = 0;
			unsigned int gradient_idx = 0;
			for (unsigned int j = 0; j < this->GradientBoundary.size(); j++) {
				gradient_idx = j;
				if (this->GradientBoundary[j].first <= this->GradientMagnitudes[i] && this->GradientMagnitudes[i] < this->GradientBoundary[j].second) {
					break;
				}
			}

			for (unsigned int j = 0; j < this->IsoValueBoundary.size(); j++) {
				isovalue_idx = j;
				if (this->IsoValueBoundary[j].first <= static_cast<int>(this->RawData[i]) && static_cast<int>(this->RawData[i]) < this->IsoValueBoundary[j].second) {
					break;
				}
			}
			int idx = static_cast<unsigned int>(((this->Interval - 1) - gradient_idx) * this->Interval + isovalue_idx);
			this->GradientHeatmap[idx] += 1;
		}
	}
	
	void IsoSurface::IsoValueHistogramEqualization() {
		this->IsEqualization = true;
		
		// 先取得舊的 histogram，並重新初始化
		std::vector<float> old_histogram = this->IsoValueHistogram;
		this->IsoValueHistogram = std::vector<float>(256, 0.0f);

		// 計算總所有 iso value 為 0 ~ 255 的顯示次數。 
		float total_iso_value = static_cast<float>(std::accumulate(old_histogram.cbegin(), old_histogram.cend(), 0.0));

		// 歸一化，所有顯示次數除以總次數，數值會介於 0 ~ 1 之間。
		std::vector<float> normalize;
		for (auto i : old_histogram) {
			normalize.push_back(i / total_iso_value);
		}

		// 計算	累計直方圖
		float current_value = 0;
		std::vector<float> cumulative;
		for (auto i : normalize) {
			current_value += i;
			cumulative.push_back(current_value);
		}

		// 再重新映射到 0 ~ 255 (並且四捨五入)
		std::vector<int> equal_values;
		for (auto i : cumulative) {
			equal_values.push_back(round(i * 255));
		}

		// 得到的數值是所謂的均衡化值 比如 equal_values[0] = 71 代表 原本  iso value = 0 均衡化後，調整為iso value = 71  數量是不變的
		for (int i = 0; i < equal_values.size(); i++) {
			this->IsoValueHistogram[equal_values[i]] += old_histogram[i];
		}

		// 最後別忘了要對整個 Volume Data 做一樣的操作（均值化）！
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

	void IsoSurface::GetGradientHeatmapAxisLabels(std::vector<std::string>& labels, bool is_axis_x) {
		labels.clear();
		if (is_axis_x) {
			float x_bin = this->IsoValueBoundary.size() / 4.0f;
			for (unsigned int i = 0; i <= 4; i++) {
				std::string temp;
				if (i == 4) {
					temp = std::to_string(static_cast<int>(this->IsoValueBoundary[(x_bin * i) - 1].second));
				} else {
					temp = std::to_string(static_cast<int>(this->IsoValueBoundary[(x_bin * i)].first));
				}
				// std::cout << temp.c_str() << std::endl;
				labels.push_back(temp);
			}
		} else {
			float y_bin = this->GradientBoundary.size() / 4.0f;
			for (unsigned int i = 0; i <= 4; i++) {
				std::string temp;
				if (i == 4) {
					temp = std::to_string(static_cast<int>(this->GradientBoundary[(y_bin * i) - 1].second));
				} else {
					temp = std::to_string(static_cast<int>(this->GradientBoundary[(y_bin * i)].first));
				}
				// std::cout << temp.c_str() << std::endl;
				labels.push_back(temp);
			}
		}
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

		if (this->CurrentRenderMode == RENDER_MODE_ISO_SURFACE) {
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
		} else if (this->CurrentRenderMode == RENDER_MODE_RAY_CASTING) {
			std::cout << "==================== Volume Rendering Information ====================" << std::endl
				<< "Raw File Path: " << this->RawDataFilePath << std::endl
				<< "Construct Method: Ray Casting" << std::endl
				<< "Voxel Count: " << GetVoxelCount() << std::endl
				<< "Gradient Magnitudes Counts: " << this->GradientMagnitudes.size() << std::endl
				<< "Elapsed time: " << this->ElapsedSeconds.count() << " (seconds)" << std::endl
				<< "================================================================================" << std::endl;
		}
	}

	void IsoSurface::Draw(Nexus::Shader* shader, glm::mat4 model) {
		if (!this->IsInitialize || !this->IsReadyToDraw) {
			Logger::Message(LOG_ERROR, "YOU MUST LOAD THE VOLUME DATA FIRST and COMPUTE THESE ISO SURFACE VERTICES.");
			return;
		}
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (this->CurrentRenderMode == RENDER_MODE_ISO_SURFACE) {
			shader->Use();
			shader->SetMat4("model", model);
			shader->SetMat3("normalModel", glm::mat3(glm::transpose(glm::inverse(model))));
			shader->SetBool("is_volume", true);
			
			// this->VAO->Bind();
			glBindVertexArray(this->VAO);
			if (this->EnableWireFrameMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			glDrawArrays(GL_TRIANGLES, 0, this->GetPositionCount());
			glBindVertexArray(0);
		}

		if (this->CurrentRenderMode == RENDER_MODE_RAY_CASTING) {
			shader->Use();
			shader->SetMat4("model", model);
			shader->SetInt("volume", 0);
			shader->SetInt("transfer_function", 1);
			shader->SetVec3("volume_resolution", this->Attributes.Resolution);
            shader->SetVec3("volume_ratio", this->Attributes.Ratio);

			// Draw a bounding-box, size will be the resolution of the volume data.
			glBindVertexArray(this->BoundingBoxVAO);
			glDrawElements(GL_TRIANGLES, (GLsizei)this->BoundingBoxIndices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}

	void IsoSurface::ComputeAllNormals(float max_gradient) {
		// 計算每一個 Voxel 的 Gradient 來當作法向量。
		for (int k = 0; k < Attributes.Resolution.z; k++) {
			for (int j = 0; j < Attributes.Resolution.y; j++) {
				for (int i = 0; i < Attributes.Resolution.x; i++) {
					glm::vec3 norm = glm::vec3(0.0f);

					if (i + 1 >= Attributes.Resolution.x) {
						// Backward difference
						norm.x = (this->GetIsoValueFromGrid(i, j, k) - this->GetIsoValueFromGrid(i - 1, j, k)) / Attributes.Ratio.x;
					} else if (i - 1 < 0) {
						// Forward difference
						norm.x = (this->GetIsoValueFromGrid(i + 1, j, k) - this->GetIsoValueFromGrid(i, j, k)) / Attributes.Ratio.x;
					} else {
						// Central difference
						norm.x = (this->GetIsoValueFromGrid(i + 1, j, k) - this->GetIsoValueFromGrid(i - 1, j, k)) / 2 * Attributes.Ratio.x;
					}

					if (j + 1 >= Attributes.Resolution.y) {
						// Backward difference
						norm.y = (this->GetIsoValueFromGrid(i, j, k) - this->GetIsoValueFromGrid(i, j - 1, k)) / Attributes.Ratio.y;
					} else if (j - 1 < 0) {
						// Forward difference
						norm.y = (this->GetIsoValueFromGrid(i, j + 1, k) - this->GetIsoValueFromGrid(i, j, k)) / Attributes.Ratio.y;
					} else {
						// Central difference
						norm.y = (this->GetIsoValueFromGrid(i, j + 1, k) - this->GetIsoValueFromGrid(i, j - 1, k)) / 2 * Attributes.Ratio.y;
					}

					if (k + 1 >= Attributes.Resolution.z) {
						// Backward difference
						norm.z = (this->GetIsoValueFromGrid(i, j, k) - this->GetIsoValueFromGrid(i, j, k - 1)) / Attributes.Ratio.z;
					} else if (k - 1 < 0) {
						// Forward difference
						norm.z = (this->GetIsoValueFromGrid(i, j, k + 1) - this->GetIsoValueFromGrid(i, j, k)) / Attributes.Ratio.z;
					} else {
						// Central difference
						norm.z = (this->GetIsoValueFromGrid(i, j, k + 1) - this->GetIsoValueFromGrid(i, j, k - 1)) / 2 * Attributes.Ratio.z;
					}

					// Gradient 長度分貝化
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
		
		// 開始一個一個 Voxel 讀取，並且每讀一個 Voxel 就抓它其他7個 Voxel (能構成一個正方形的)，
		// 一次輸入 8 個 Voxel，檢查並求出正方塊中所包覆的三角形頂點與法向量為何。
		for (int k = 0; k < Attributes.Resolution.z - 1; k++) {
			for (int j = 0; j < Attributes.Resolution.y - 1; j++) {
				for (int i = 0; i < Attributes.Resolution.x - 1; i++) {
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
				this->AddPosition(position_list[this->TriangleTable[cube_index][i + offset]] * this->Attributes.Ratio);
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
