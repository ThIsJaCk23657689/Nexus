#pragma once

#include <glad/glad.h>
#include <cstdint>
#include <cstddef>

namespace Nexus {
	class VertexBuffer {
	public:
		VertexBuffer(void* vertices, std::size_t size);
		~VertexBuffer();

		inline void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, this->ID); }
		inline void Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
		
	private:
		GLuint ID;
		
	};

	class IndexBuffer {
	public:
		IndexBuffer(void* indices, std::size_t size);
		~IndexBuffer();

		inline void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID); }
		inline void Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

	private:
		GLuint ID;

	};

	class VertexAttribute {
	public:
		GLuint Dims;
		uint64_t Offset;
	};

	class VertexArray {
	public:
		VertexArray(VertexBuffer* vbo, VertexAttribute* attribs, GLuint attribsCount, GLsizei stride, IndexBuffer* ebo = nullptr);
		~VertexArray();

		inline void Bind() const { glBindVertexArray(this->ID); }
		inline void Unbind() const { glBindVertexArray(0); }
		
	private:
		GLuint ID;
		
	};

	class Mesh {
	public:
		Mesh(const float* vertices, unsigned int vcount);
		Mesh(const float* vertices, unsigned int vcount, const unsigned int* indices, unsigned int icount);
		~Mesh();

		inline void BindVAO() const { glBindVertexArray(this->VAO); }
		inline void UnbindVAO() const { glBindVertexArray(0); }
		
	private:
		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
		
	};
}