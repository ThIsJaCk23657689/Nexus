#pragma once

#include "Object.h"

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
		if (ebo != nullptr) {
			ebo->Bind();
		}
		
		for (GLuint i = 0; i < attribs_count; i++) {
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, attribs[i].Dims, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)attribs[i].Offset);
		}
		this->Unbind();
	}

	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &this->ID);
	}
	
	void Object::addPosition(float x, float y, float z) {
		this->Position.push_back(x);
		this->Position.push_back(y);
		this->Position.push_back(z);

		this->Vertices.push_back(x);
		this->Vertices.push_back(y);
		this->Vertices.push_back(z);
	}

	void Object::addNormal(float nx, float ny, float nz) {
		this->Normal.push_back(nx);
		this->Normal.push_back(ny);
		this->Normal.push_back(nz);

		this->Vertices.push_back(nx);
		this->Vertices.push_back(ny);
		this->Vertices.push_back(nz);
	}

	void Object::addTexCoord(float u, float v) {
		this->TexCoord.push_back(u);
		this->TexCoord.push_back(v);

		this->Vertices.push_back(u);
		this->Vertices.push_back(v);
	}

	void Object::addIndices(unsigned int i1, unsigned int i2, unsigned int i3) {
		this->Indices.push_back(i1);
		this->Indices.push_back(i2);
		this->Indices.push_back(i3);
	}
}