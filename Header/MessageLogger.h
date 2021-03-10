#pragma once

#include <glad.h>
#include <string>

namespace Nexus {

	// This is for the argument "type" which in the message().
	enum LogLevel {
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR,
	};
	
	class MessageLogger {
	public:
		void Message(LogLevel type, std::string message);
		void ShowInitInfo(const GLubyte* vendor, const GLubyte* renderer, const GLubyte* version);
		
	private:
		
		std::string GetTimestamp();
		
	};
}