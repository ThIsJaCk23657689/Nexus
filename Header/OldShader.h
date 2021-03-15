#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Logger.h";

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Nexus {
	class OldShader {
	public:
		unsigned int ID;

		OldShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr) {
			std::string vertexCode;
			std::string fragmentCode;
			std::string geometryCode;

			std::ifstream vShaderFile;
			std::ifstream fShaderFile;
			std::ifstream gShaderFile;

			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			try {
				// Open files
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);
				std::stringstream vShaderStream, fShaderStream;

				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();

				vShaderFile.close();
				fShaderFile.close();

				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();

				if (geometryPath != nullptr) {
					gShaderFile.open(geometryPath);
					std::stringstream gShaderStream;
					gShaderStream << gShaderFile.rdbuf();
					gShaderFile.close();
					geometryCode = gShaderStream.str();
				}
			} catch (std::ifstream::failure& e) {
				// Handle Failure
				Logger::Message(LOG_ERROR, "[ERROR] Failed to load shader files.");
			}
			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();

			// Compile these shaders.
			unsigned int vertex, fragment;
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			checkCompileErrors(vertex, "Vertex", vertexPath);

			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			checkCompileErrors(fragment, "Fragment", fragmentPath);

			unsigned int geometry;
			if (geometryPath != nullptr) {
				const char* gShaderCode = geometryCode.c_str();
				geometry = glCreateShader(GL_GEOMETRY_SHADER);
				glShaderSource(geometry, 1, &gShaderCode, NULL);
				glCompileShader(geometry);
				checkCompileErrors(geometry, "Geometry", geometryPath);
			}

			ID = glCreateProgram();
			glAttachShader(ID, vertex);
			glAttachShader(ID, fragment);
			if (geometryPath != nullptr) {
				glAttachShader(ID, geometry);
			}
			glLinkProgram(ID);
			checkCompileErrors(ID, "Program", NULL);

			glDeleteShader(vertex);
			glDeleteShader(fragment);
			if (geometryPath != nullptr) {
				glDeleteShader(geometry);
			}
		}

		// Util functions

		void use() {
			glUseProgram(ID);
		}

		void setBool(const std::string& name, bool value) const {
			glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
		}

		void setInt(const std::string& name, int value) const {
			glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
		}

		void setFloat(const std::string& name, float value) const {
			glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
		}

		void setVec3(const std::string& name, glm::vec3 vector) const {
			glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &vector[0]);
		}

		void setVec3(const std::string& name, float x, float y, float z) const {
			glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
		}

		void setVec4(const std::string& name, glm::vec4 vector) const {
			glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &vector[0]);
		}

		void setVec4(const std::string& name, float x, float y, float z, float w) const {
			glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
		}

		void setMat3(const std::string& name, glm::mat3 matrices) const {
			glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &matrices[0][0]);
		}

		void setMat4(const std::string& name, glm::mat4 matrices) const {
			glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &matrices[0][0]);
		}

	private:
		void checkCompileErrors(unsigned int shader, std::string type, const char* filePath) {
			int success;
			char infoLog[1024];
			if (type != "Program") {
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success) {
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);
					Logger::Message(LOG_ERROR, "Failed to compile shader, type: " + type + ", filepath: " + filePath);
					Logger::Message(LOG_ERROR, infoLog);
				}
			} else {
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success) {
					glGetProgramInfoLog(shader, 1024, NULL, infoLog);
					Logger::Message(LOG_ERROR, "Failed to link shader program, type: " + type);
					Logger::Message(LOG_ERROR, infoLog);
				}
			}
		}
	};
}
