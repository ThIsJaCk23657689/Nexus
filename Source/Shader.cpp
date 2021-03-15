#include "Shader.h"
#include "Logger.h"

#include <fstream>
#include <sstream>

namespace Nexus {
	Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {

		GLuint vertex = CreateShader(vertexPath, ShaderType::Vertex);
		GLuint fragment = CreateShader(fragmentPath, ShaderType::Fragment);

		GLuint geometry = NULL;
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
		glUniform1i(this->GetUniformLocation(name.c_str()), value);
	}

	void Shader::SetInt(const std::string& name, GLint value) {
		glUniform1i(this->GetUniformLocation(name.c_str()), value);
	}

	void Shader::SetUnsignedInt(const std::string& name, GLuint value) {
		glUniform1i(this->GetUniformLocation(name.c_str()), value);
	}

	void Shader::SetFloat(const std::string& name, GLfloat value) {
		glUniform1f(this->GetUniformLocation(name.c_str()), value);
	}

	void Shader::SetVec3(const std::string& name, glm::vec3 vector) {
		glUniform3fv(this->GetUniformLocation(name.c_str()), 1, &vector[0]);
	}

	void Shader::SetVec3(const std::string& name, GLfloat x, GLfloat y, GLfloat z) {
		glUniform3f(this->GetUniformLocation(name.c_str()), x, y, z);
	}

	void Shader::SetVec4(const std::string& name, glm::vec4 vector) {
		glUniform4fv(this->GetUniformLocation(name.c_str()), 1, &vector[0]);
	}

	void Shader::SetVec4(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
		glUniform4f(this->GetUniformLocation(name.c_str()), x, y, z, w);
	}

	void Shader::SetMat3(const std::string& name, glm::mat3 matrices) {
		glUniformMatrix3fv(this->GetUniformLocation(name.c_str()), 1, GL_FALSE, &matrices[0][0]);
	}

	void Shader::SetMat4(const std::string& name, glm::mat4 matrices) {
		glUniformMatrix4fv(this->GetUniformLocation(name.c_str()), 1, GL_FALSE, &matrices[0][0]);
	}

	GLuint Shader::CreateShader(const char* path, ShaderType type) {

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
			Logger::Message(LOG_ERROR, "[ERROR] Failed to load shader files, type: " + this->GetShaderTypeString(type) + ", filepath: " + path);
			return GLuint(-1);
		}
		
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
		}

		// Compile these shaders.
		GLuint Shader = glCreateShader(CurrentShaderType);
		glShaderSource(Shader, 1, &ShaderCode, 0);
		glCompileShader(Shader);

		if (this->CheckCompileErrors(Shader, type, path)) {
			return GLuint(-1);
		}

		return Shader;
	}

	GLint Shader::GetUniformLocation(const char* name) {
		if (UniformLocationCache.find(name) != UniformLocationCache.end()) {
			return UniformLocationCache[name];
		}

		GLint location = glGetUniformLocation(this->ID, name);
		if (location == -1) {
			Logger::Message(LOG_WARNING, "The uniform <" + std::string(name) + "> doesn't exist!");
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
				Logger::Message(LOG_ERROR, "Failed to link shader program, type: " + this->GetShaderTypeString(type));
				Logger::Message(LOG_ERROR, infoLog);
				return true;
			}
		} else {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				char infoLog[1024];
				glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
				Logger::Message(LOG_ERROR, "Failed to compile shader, type: " + this->GetShaderTypeString(type) + ", filepath: " + filePath);
				Logger::Message(LOG_ERROR, infoLog);
				return true;
			}
		}
		return false;
	}

	std::string Shader::GetShaderTypeString(ShaderType type) {
		std::string result = "Vertex";
		switch (type) {
		case ShaderType::Fragment:
			result = "Fragment";
			break;
		case ShaderType::Geometry:
			result = "Geometry";
			break;
		case ShaderType::Program:
			result = "Program";
			break;
		}

		return result;
	}
}