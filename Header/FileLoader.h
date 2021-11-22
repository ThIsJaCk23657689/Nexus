#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <regex>

#include "IsoSurface.h"
#include "Logger.h"
#include "Utill.h"

namespace Nexus {
	class FileLoader {
	public:

	    static void LoadHighDimensionData(const std::string& filepath, std::vector<GLfloat>& data, GLuint& count, GLuint& dims) {
            std::ifstream file;
            file.open(filepath, std::ios::in);
            if (file.fail()) {
                file.close();
                Nexus::Logger::Message(LOG_ERROR, "Failed to load the high dimension data at: " + filepath);
            }

            std::string data_count, data_dims, data_temp;
            file >> data_count;
            file >> data_dims;

            data.clear();
            while (file >> data_temp) {
                data.push_back(std::stof(data_temp));
            }
            file.close();

            Nexus::Logger::Message(LOG_INFO, "The size of the vector: " + std::to_string(data.size()));
            Nexus::Logger::Message(LOG_INFO, "The size of the high dimension data: " + data_count + " x " + data_dims);

            count = std::stoi(data_count);
            dims = std::stoi(data_dims);
	    }

        static std::vector<std::string> GetAllFilesNamesWithinFolder(const std::string& folder_path, const std::string& file_extension) {
            std::vector<std::string> file_names;
            for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
			    if (entry.path().extension() == "." + file_extension) {
                    file_names.push_back(entry.path().filename().string());
			    }
            }
            return file_names;
        }


        static std::string LoadInfoFile(const std::string& path) {
            std::ifstream info_file;
            std::stringstream info_stream;
            info_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try {
                // Open files and loaded.
                info_file.open(path);
                info_stream << info_file.rdbuf();
                info_file.close();
            } catch (std::ifstream::failure& e) {
                // Handle Failure
                Nexus::Logger::Message(LOG_ERROR, "Failed to load info file. Filepath: " + path);
                exit(-1);
            }
            const std::string& info_source = info_stream.str();

            return info_source;
        }

        static void LoadRawFile(std::vector<float>& raw_data, const std::string& path, const IsoSurfaceAttributes& attributes) {
            size_t raw_file_size = std::filesystem::file_size(path);
            std::cout << "Raw File Size (Bytes): " << raw_file_size << std::endl;

            std::vector<uint8_t> buffer(raw_file_size / sizeof(uint8_t));

//            if (attributes.DataType == VolumeDataType_UnsignedChar) {
//                std::vector<uint8_t> buffer(raw_file_size / sizeof(uint8_t));
//            } else if (attributes.DataType == VolumeDataType_UnsignedShort) {
//                std::vector<uint16_t> buffer(raw_file_size / sizeof(uint16_t));
//            } else if (attributes.DataType == VolumeDataType_UnsignedInt) {
//                std::vector<uint32_t> buffer(raw_file_size / sizeof(uint32_t));
//            } else if (attributes.DataType == VolumeDataType_UnsignedLong) {
//                std::vector<uint64_t> buffer(raw_file_size / sizeof(uint64_t));
//            }

            std::ifstream file(path, std::ios::binary);
            if (file.fail()) {
                Nexus::Logger::Message(LOG_ERROR, "FAILED TO LOAD THE RAW FILE, PLEASE CHECK THE FILE EXISTS IN THE CORRECT PATH");
                Nexus::Logger::Message(LOG_ERROR, "FILE PATH: " + path);
                exit(-1);
            }
            file.read(reinterpret_cast<char*>(buffer.data()), raw_file_size);
            file.close();

            raw_data.assign(buffer.begin(), buffer.end());
        }

        static std::stringstream LoadShaderFile(const char* path, std::string shader_type) {
            std::ifstream ShaderFile;
            std::stringstream ShaderStream;
            ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try {
                // Open files and loaded.
                ShaderFile.open(path);
                ShaderStream << ShaderFile.rdbuf();
                ShaderFile.close();
            } catch (std::ifstream::failure& e) {
                // Handle Failure
                Nexus::Logger::Message(LOG_ERROR, "Failed to load " + shader_type + " shader files. Filepath: " + path);
                exit(-1);
            }
            return ShaderStream;
        }

        static void OutputTransferFunction(const char* path, std::vector<float> transfer, IsoSurface* iso_surface) {
            Logger::Message(LOG_INFO, path);

            std::ofstream MyFile(path);
            MyFile << "Raw File Path: " << iso_surface->GetRawDataFilePath() << std::endl;
            MyFile << "Inf File Path: " << iso_surface->GetRawDataFilePath() << std::endl;
            MyFile << "Equalization: " << iso_surface->GetIsEqualization() << std::endl;
            MyFile << "Resolutions: " << "(" << iso_surface->GetResolution().x << ", " << iso_surface->GetResolution().y << ", " << iso_surface->GetResolution().z << ")" << std::endl;
            MyFile << "Data Value\t\tRed\t\tGreen\t\tBlue\t\tAlpha\t\t\n";
            for (unsigned i = 0; i < transfer.size(); i = i + 4) {
                MyFile << (i / 4.0f) << "\t\t\t" << round(transfer[i] * 100) / 100.0f << "\t\t" << round(transfer[i + 1] * 100) / 100.0f << "\t\t" << round(transfer[i + 2] * 100) / 100.0f << "\t\t" << round(transfer[i + 3] * 100) / 100.0f << "\t\t\n";
            }
            MyFile.close();
        }
    private:

    };
}
