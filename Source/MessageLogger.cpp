#include "MessageLogger.h"

#include <iostream>
#include <ctime>

namespace Nexus {
	void MessageLogger::Message(LogLevel type, std::string message) {
		switch (type) {
			case LOG_DEBUG:
				std::cout << GetTimestamp() << "[DEBUG] " << message << std::endl;
				break;
			case LOG_INFO:
				std::cout << GetTimestamp() << "[INFO] " << message << std::endl;
				break;
			case LOG_WARNING:
				std::cout << GetTimestamp() << "[WARNING] " << message << std::endl;
				break;
			case LOG_ERROR:
				std::cerr << GetTimestamp() << "[ERROR] " << message << std::endl;
				break;
			default:
				std::cerr << GetTimestamp() << "[ERROR] " << "Undefined type in Logger.Message()" << std::endl;
				break;
		}
	}

	void MessageLogger::ShowInitInfo(const GLubyte* vendor, const GLubyte* renderer, const GLubyte* version) {
		std::cout << GetTimestamp() << "[INFO] GPU Vendor: " << vendor << std::endl;
		std::cout << GetTimestamp() << "[INFO] GPU: " << renderer << std::endl;
		std::cout << GetTimestamp() << "[INFO] OpenGL Version: " << version << std::endl;
	}

	// Generate timestamp for logging
	std::string MessageLogger::GetTimestamp() {
		time_t timer = std::time(0);
		std::tm bt{};
		localtime_s(&bt, &timer);

		char buffer[64];
		return { buffer, std::strftime(buffer, sizeof(buffer), "%F %T ", &bt) };
	}
}