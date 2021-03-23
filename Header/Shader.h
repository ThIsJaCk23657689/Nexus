#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>

namespace Nexus {
	class Shader {
	public:
		Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
		~Shader();

		void Use() const {
			glUseProgram(this->ID);
		}

		void Unused() const {
			glUseProgram(0);
		}

		void SetBool(const std::string& name, GLboolean value);

		void SetInt(const std::string& name, GLint value);

		void SetUnsignedInt(const std::string& name, GLuint value);
		
		void SetFloat(const std::string& name, GLfloat value);

		void SetVec3(const std::string& name, glm::vec3 vector);

		void SetVec3(const std::string& name, GLfloat x, GLfloat y, GLfloat z);

		void SetVec4(const std::string& name, glm::vec4 vector);

		void SetVec4(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

		void SetMat3(const std::string& name, glm::mat3 matrices);

		void SetMat4(const std::string& name, glm::mat4 matrices);
		
	private:
		enum class ShaderType {
			Vertex,
			Fragment,
			Geometry,
			Program
		};

		GLuint ID;
		std::unordered_map<std::string, GLuint> UniformLocationCache;
		
		GLuint CreateShader(const char* path, ShaderType type);
		GLint GetUniformLocation(const std::string& name);
		bool CheckCompileErrors(GLuint shader, ShaderType type, const char* filePath);
		std::string GetShaderTypeString(ShaderType type);
	};
}
