#pragma once
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <vector>
#include "Logger.h"

namespace Nexus {
	class FileLoader {
	public:
		static std::vector<std::string> GetAllFilesNamesWithinFolder(const std::string& folder_path, std::string search_keyword) {
			std::vector<std::string> file_names;
			std::string search_path = folder_path + search_keyword;

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

			if(file_names.empty()) {
				Nexus::Logger::Message(LOG_WARNING, "There is no any volume data files in the folder. :(");
			} else {
				Nexus::Logger::Message(LOG_INFO, "Found " + std::to_string(file_names.size()) + " files.");
			}
			
			return file_names;
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
	private:

	};
}
