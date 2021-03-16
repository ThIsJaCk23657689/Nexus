#include "Mesh.h"

#include <iostream>

namespace Nexus {
	VertexBuffer::VertexBuffer(void* vertices, std::size_t size) {
		glGenBuffers(1, &this->ID);
		this->Bind();
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		this->Unbind();
	}

	VertexBuffer::~VertexBuffer() {
		glDeleteBuffers(1, &this->ID);
	}

	IndexBuffer::IndexBuffer(void* indices, std::size_t size) {
		glGenBuffers(1, &this->ID);
		this->Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
		this->Unbind();
	}

	IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &this->ID);
	}

	VertexArray::VertexArray(VertexBuffer* vbo, VertexAttributes* attribs, GLuint attribs_count, GLsizei stride, IndexBuffer* ebo) {

		glGenVertexArrays(1, &this->ID);

		this->Bind();
		vbo->Bind();
		if(ebo) {
			ebo->Bind();
		}

		for (GLuint i = 0; i < attribs_count; i++) {
			// std::cout << attribs[i].Dims << "\t" << stride << "\t" << attribs[i].Offset << std::endl;
			glVertexAttribPointer(i, attribs[i].Dims, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)attribs[i].Offset);
			glEnableVertexAttribArray(i);
		}

		vbo->Unbind();
		if (ebo) {
			ebo->Unbind();
		}
	}

	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &this->ID);
	}

	Mesh::Mesh(const float* vertices, unsigned int vcount) {
		glGenVertexArrays(1, &this->VAO);
		this->BindVAO();

		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, vcount * sizeof(float), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		this->UnbindVAO();
	}

	Mesh::Mesh(const float* vertices, unsigned int vcount, const unsigned int* indices, unsigned int icount) {
		glGenVertexArrays(1, &this->VAO);
		this->BindVAO();

		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, vcount * sizeof(float), vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &this->EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, icount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		this->UnbindVAO();
	}

	Mesh::~Mesh() {
		glDeleteVertexArrays(1, &this->VAO);
		glDeleteBuffers(1, &this->VBO);
		glDeleteBuffers(1, &this->EBO);
	}
}
