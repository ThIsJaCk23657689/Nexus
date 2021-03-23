#pragma once

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture2D.h"

#include <cstdint>
#include <cstddef>
#include <vector>
#include <memory>

constexpr float PI = 3.14159265359f;

namespace Nexus {

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};

	class VertexBuffer {
	public:
		VertexBuffer(void* vertices, std::size_t size);
		~VertexBuffer();

		GLuint GetID() const { return this->ID; }

		inline void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, this->ID); }
		inline void Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

	private:
		GLuint ID;
	};

	class IndexBuffer {
	public:
		IndexBuffer(void* indices, std::size_t size);
		~IndexBuffer();

		GLuint GetID() const { return this->ID; }

		inline void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID); }
		inline void Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

	private:
		GLuint ID;
	};

	struct VertexAttributes {
		GLuint Dims;
		uint64_t Offset;
	};

	class VertexArray {
	public:
		VertexArray(VertexBuffer* vbo, VertexAttributes* attribs, GLuint attribs_count, GLsizei stride, IndexBuffer* ebo = nullptr);
		~VertexArray();

		GLuint GetID() const { return this->ID; }

		inline void Bind() const { glBindVertexArray(this->ID); }
		inline void Unbind() const { glBindVertexArray(0); }

	private:
		GLuint ID;
	};
	
	class Object {
	public:

		Object() {
			this->Texture.resize(16, nullptr);
		}

		void Draw(Nexus::Shader* shader);
		void Debug();
		
		virtual ~Object() {}

		void SetWireFrameMode(bool enable) {
			this->EnableWireFrameMode = enable;
		}

		void SetTextureFill(bool enable) {
			this->EnableTextureFill = enable;
		}

		void SetColor(glm::vec3 color) {
			this->Color = color;
			this->EnableTextureFill = false;
		}

		void SetColor(float r, float g, float b) {
			this->Color = glm::vec3(r, g, b);
			this->EnableTextureFill = false;
		}

		void SetTexture(GLenum unit, Texture2D* texture) {
			this->Texture[unit] = texture;
			this->EnableTextureFill = true;
		}
		
		unsigned int GetVertexCount() const { return (unsigned int)this->Vertices.size() / 8; }
		unsigned int GetPositionCount() const { return (unsigned int)this->Position.size() / 3; }
		unsigned int GetNormalCount() const { return (unsigned int)this->Normal.size() / 3; }
		unsigned int GetTexCoordCount() const { return (unsigned int)this->TexCoord.size() / 2; }
		unsigned int GetIndexCount() const { return (unsigned int)this->Indices.size(); }
		
	protected:
		std::vector<float> Vertices;
		std::vector<float> Position;
		std::vector<float> Normal;
		std::vector<float> TexCoord;
		std::vector<unsigned int> Indices;
		unsigned int VertexCount = 0;

		std::string ShapeName;
		bool EnableWireFrameMode = false;
		bool EnableTextureFill = false;
		glm::vec3 Color = glm::vec3(0.5f, 0.4f, 0.3f);
		std::vector<Texture2D*> Texture;

		std::unique_ptr<Nexus::VertexArray> VAO;
		std::unique_ptr<Nexus::VertexBuffer> VBO;
		std::unique_ptr<Nexus::IndexBuffer> EBO;

		void Initialize() {
			this->GenerateVertices();
			this->BufferInitialize();
		}
		
		virtual void GenerateVertices() = 0;
		virtual void BufferInitialize();
		
		void AddPosition(float x, float y, float z);
		void AddNormal(float nx, float ny, float nz);
		void AddTexCoord(float u, float v);
		void AddIndices(unsigned int i1, unsigned int i2, unsigned int i3);
	};
}
