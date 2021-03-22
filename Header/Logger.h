#pragma once

#include <glad.h>
#include <string>

namespace Nexus {

	const std::string NexusVerison = "0.1.7";
	
	// This is for the argument "type" which in the message().
	enum LogLevel {
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR,
	};
	
	class Logger {
	public:
		static void Message(LogLevel type, std::string message);
		static void ShowInitInfo(const GLubyte* vendor, const GLubyte* renderer, const GLubyte* version);
		static void ShowMe();
		static std::string GetTimestamp();

	};
}