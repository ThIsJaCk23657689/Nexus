#include "Shader.h"
#include "Logger.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "FileLoader.h"

namespace Nexus {
	Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {

		GLuint vertex = CreateShader(vertexPath, ShaderType::Vertex);
		GLuint fragment = CreateShader(fragmentPath, ShaderType::Fragment);

		GLuint geometry = 0;
		if (geometryPath != nullptr) {
			geometry = CreateShader(geometryPath, ShaderType::Geometry);
		}

		this->ID = glCreateProgram();
		glAttachShader(this->ID, vertex);
		glAttachShader(this->ID, fragment);
		if (geometryPath != nullptr) {
			glAttachShader(this->ID, geometry);
		}
		glLinkProgram(this->ID);

		if (this->CheckCompileErrors(this->ID, ShaderType::Program, nullptr)) {
			exit(1);
		}
		
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr) {
			glDeleteShader(geometry);
		}
	}

	Shader::~Shader() {
		glDeleteProgram(this->ID);
	}

	void Shader::SetBool(const std::string& name, GLboolean value) {
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetInt(const std::string& name, GLint value) {
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUnsignedInt(const std::string& name, GLuint value) {
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetFloat(const std::string& name, GLfloat value) {
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetVec2(const std::string& name, glm::vec2 vector) {
		glUniform2fv(GetUniformLocation(name), 1, &vector[0]);
	}

	void Shader::SetVec2(const std::string& name, GLfloat x, GLfloat y) {
		glUniform2f(GetUniformLocation(name), x, y);
	}

	void Shader::SetVec3(const std::string& name, glm::vec3 vector) {
		glUniform3fv(GetUniformLocation(name), 1, &vector[0]);
	}

	void Shader::SetVec3(const std::string& name, GLfloat x, GLfloat y, GLfloat z) {
		glUniform3f(GetUniformLocation(name), x, y, z);
	}

	void Shader::SetVec4(const std::string& name, glm::vec4 vector) {
		glUniform4fv(GetUniformLocation(name), 1, &vector[0]);
	}

	void Shader::SetVec4(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
		glUniform4f(GetUniformLocation(name), x, y, z, w);
	}

	void Shader::SetMat3(const std::string& name, glm::mat3 matrices) {
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &matrices[0][0]);
	}

	void Shader::SetMat4(const std::string& name, glm::mat4 matrices) {
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrices[0][0]);
	}

	GLuint Shader::CreateShader(const char* path, ShaderType type) {

		std::stringstream ShaderStream = FileLoader::LoadShaderFile(path, this->GetShaderTypeString(type));
		
		const std::string& ShaderSource = ShaderStream.str();
		const char* ShaderCode = ShaderSource.c_str();
		
		GLenum CurrentShaderType = GL_VERTEX_SHADER;
		switch (type) {
			case ShaderType::Fragment:
				CurrentShaderType = GL_FRAGMENT_SHADER;
				break;
			case ShaderType::Geometry:
				CurrentShaderType = GL_GEOMETRY_SHADER;
				break;
            default:
                break;
		}

		// Compile these shaders.
		GLuint Shader = glCreateShader(CurrentShaderType);
		glShaderSource(Shader, 1, &ShaderCode, 0);
		glCompileShader(Shader);

		if (this->CheckCompileErrors(Shader, type, path)) {
			return GLuint(-1);
		}

		// Print the shader code on the screen. (Debug use)
		// Logger::Message(LOG_DEBUG, "The Shader Code: \n" + ShaderSource);
		
		return Shader;
	}

	GLint Shader::GetUniformLocation(const std::string& name) {
		if (UniformLocationCache.find(name) != UniformLocationCache.end()) {
			return UniformLocationCache[name];
		}

		GLint location = glGetUniformLocation(this->ID, name.c_str());
		if (location == -1) {
			Logger::Message(LOG_WARNING, "The uniform variable <" + name + "> doesn't exist in this shader ID: " + std::to_string(this->ID));
		}

		UniformLocationCache[name] = location;
		return location;
	}

	bool Shader::CheckCompileErrors(GLuint shader, ShaderType type, const char* filePath) {
		GLint success;
		if (type == ShaderType::Program) {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				char infoLog[1024];
				glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
				Logger::Message(LOG_ERROR, "Failed to link a shader program. :(");
				Logger::Message(LOG_ERROR, infoLog);
				return true;
			}
			Logger::Message(LOG_DEBUG, "Created a shader program successfully, Shader ID: " + std::to_string(this->ID));
		} else {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				char infoLog[1024];
				glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
				Logger::Message(LOG_ERROR, "Failed to compile a " + this->GetShaderTypeString(type) + " shader. Filepath: " + filePath);
				Logger::Message(LOG_ERROR, infoLog);
				return true;
			}
			Logger::Message(LOG_DEBUG, "Compiled a " + this->GetShaderTypeString(type) + " shader successfully. Filepath: " + filePath);
		}
		return false;
	}

	std::string Shader::GetShaderTypeString(ShaderType type) {
		std::string result = "vertex";
		switch (type) {
            case ShaderType::Fragment:
                result = "fragment";
                break;
            case ShaderType::Geometry:
                result = "geometry";
                break;
            case ShaderType::Program:
                result = "program";
                break;
            default:
                break;
		}

		return result;
	}
}
