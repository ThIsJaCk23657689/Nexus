#pragma once

#include "Logger.h"

#include <fstream>
#include <sstream>
#include <vector>

namespace Nexus {
	class FileLoader {
	public:
		static std::vector<unsigned char> LoadRawFile(const std::string& path) {
			std::ifstream file(path, std::ios::binary);
			std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});
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
	};
}
