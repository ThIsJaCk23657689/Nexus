#pragma once

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include <cstdint>
#include <cstddef>
#include <vector>
#include <memory>

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

		virtual void Initialize() = 0;
		virtual void Draw(Nexus::Shader* shader) = 0;
		virtual void Debug() = 0;
		
		virtual ~Object() {}

		void setWireFrameMode(bool enable) {
			this->EnableWireFrameMode = enable;
		}

		void setColor(glm::vec3 color) {
			this->Color = color;
		}

		void setColor(float r, float g, float b) {
			this->Color = glm::vec3(r, g, b);
		}
		
		unsigned int getVertexCount() const { return (unsigned int)this->Vertices.size() / 8; }
		unsigned int getPositionCount() const { return (unsigned int)this->Position.size() / 3; }
		unsigned int getNormalCount() const { return (unsigned int)this->Normal.size() / 3; }
		unsigned int getTexCoordCount() const { return (unsigned int)this->TexCoord.size() / 2; }
		unsigned int getIndexCount() const { return (unsigned int)this->Indices.size(); }
		
	protected:
		std::vector<float> Vertices;
		std::vector<float> Position;
		std::vector<float> Normal;
		std::vector<float> TexCoord;
		std::vector<unsigned int> Indices;
		unsigned int VertexCount = 0;

		bool EnableWireFrameMode = false;
		glm::vec3 Color = glm::vec3(0.5f, 0.4f, 0.3f);

		std::unique_ptr<Nexus::VertexArray> VAO;
		std::unique_ptr<Nexus::VertexBuffer> VBO;
		std::unique_ptr<Nexus::IndexBuffer> EBO;

		void addPosition(float x, float y, float z);
		void addNormal(float nx, float ny, float nz);
		void addTexCoord(float u, float v);
		void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);
	};
}