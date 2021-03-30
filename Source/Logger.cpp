#include "Logger.h"

#include <iostream>
#include <chrono>
#include <ctime>

namespace Nexus {

	const std::string NexusVerison = "0.2.7";
	
	void Logger::Message(LogLevel type, std::string message) {
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

	void Logger::ShowInitInfo(const GLubyte* vendor, const GLubyte* renderer, const GLubyte* version) {
		std::cout << GetTimestamp() << "[INFO] GPU Vendor: " << vendor << std::endl;
		std::cout << GetTimestamp() << "[INFO] GPU Renderer: " << renderer << std::endl;
		std::cout << GetTimestamp() << "[INFO] OpenGL Version: " << version << std::endl;
	}

	void Logger::ShowMe() {
		std::cout << std::endl;
		std::cout << "  $$\\   $$\\" << std::endl;
		std::cout << "  $$$\\  $$ |" << std::endl;
		std::cout << "  $$$$\\ $$ | $$$$$$\\  $$\\   $$\\ $$\\   $$\\  $$$$$$$\\" << std::endl;
		std::cout << "  $$ $$\\$$ |$$  __$$\\ \\$$\\ $$  |$$ |  $$ |$$  _____|" << std::endl;
		std::cout << "  $$ \\$$$$ |$$$$$$$$ | \\$$$$  / $$ |  $$ |\\$$$$$$\\" << std::endl;
		std::cout << "  $$ |\\$$$ |$$   ____| $$  $$\\  $$ |  $$ | \\____$$\\" << std::endl;
		std::cout << "  $$ | \\$$ |\\$$$$$$$\\ $$  /\\$$\\ \\$$$$$$  |$$$$$$$  |" << std::endl;
		std::cout << "  \\__|  \\__| \\_______|\\__/  \\__| \\______/ \\_______/" << std::endl;
		std::cout << std::endl;
		std::cout << "\t\t  ________    ____         __      ________  " << std::endl;
		std::cout << "Created by: \t /_  __/ /_  /  _/____    / /___ _/ ____/ /__" << std::endl;
		std::cout << "\t\t  / / / __ \\ / // ___/_  / / __ `/ /   / //_/" << std::endl;
		std::cout << "\t\t / / / / / // /(__  ) /_/ / /_/ / /___/ ,<   " << std::endl;
		std::cout << "Version: " + NexusVerison + "\t/_/ /_/ /_/___/____/\\____/\\__,_/\\____/_/|_|  " << std::endl;
		std::cout << std::endl;
	}

	// Generate timestamp for logging
	std::string Logger::GetTimestamp() {
		std::time_t now = std::time(0);
		struct tm  tstruct;
		char buffer[80];
		tstruct = *localtime(&now);
		std::strftime(buffer, sizeof(buffer), "%F %T ", &tstruct);

		return buffer;
	}
}