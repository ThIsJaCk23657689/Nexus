#pragma once
#include <fstream>
#include <sstream>
#include <vector>

namespace Nexus {
	class FileLoader {
	public:
		static std::vector<unsigned char> load(const char* path) {
			std::ifstream file(path, std::ios::binary);
			std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});
			file.close();
			return buffer;
		}
	};
}
