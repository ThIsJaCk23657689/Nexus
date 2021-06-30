#pragma once
#ifdef _WIN32 || _WIN64
    #include <Windows.h>
#endif
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
		static std::vector<std::string> GetAllFilesNamesWithinFolder(const std::string& folder_path, std::string search_keyword) {
			std::vector<std::string> file_names;
			std::string search_path = folder_path + search_keyword;

#ifdef _WIN32 || _WIN64
            WIN32_FIND_DATA fd;
			HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					// read all (real) files in current folder
					// , delete '!' read other 2 default folder . and ..
					if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						file_names.push_back(fd.cFileName);
					}
				} while (::FindNextFile(hFind, &fd));
				::FindClose(hFind);
			}
#endif

#ifdef __APPLE__
			for (const auto& entry : std::filesystem::directory_iterator(search_path)) {
			    file_names.push_back(entry.path());
			}
#endif
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
		
		static std::vector<unsigned char> LoadRawFile(const std::string& path) {
			std::vector<unsigned char> buffer;
			std::ifstream file(path, std::ios::binary);
			if (file.good()) {
				buffer = std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});
			} else {
				Nexus::Logger::Message(LOG_ERROR, "FAILED TO LOAD THE RAW FILE, PLEASE CHECK THE FILE EXISTS IN THE CORRECT PATH");
				Nexus::Logger::Message(LOG_ERROR, "FILE PATH: " + path);
				exit(-1);
			}
			file.close();
			return buffer;
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

		static void OutputTransferFunction(const char* path, std::vector<float> &transfer, IsoSurface* iso_surface) {
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
