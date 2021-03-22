#include "Object.h"

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

	void Object::Debug() {
		std::cout << "===== " + this->ShapeName + " =====\n"
			<< "Vertex Count: " << GetVertexCount() << std::endl
			<< "Position Count: " << GetPositionCount() << std::endl
			<< "Normal Count: " << GetNormalCount() << std::endl
			<< "TexCoord Count: " << GetTexCoordCount() << std::endl
			<< "Index Count: " << GetIndexCount() << std::endl;
	}

	void Object::Draw(Nexus::Shader* shader) {
		shader->Use();
		shader->SetBool("enable_texture", this->EnableTextureFill);
		if (this->EnableTextureFill) {
			shader->SetInt("texture0", 0);

			for (int i = 0; i < this->Texture.size(); i++) {
				if (this->Texture[i] == nullptr) {
					continue;
				}
				this->Texture[i]->Bind();

			}
		} else {
			shader->SetVec3("color", this->Color);
		}

		this->VAO->Bind();
		if (this->EnableWireFrameMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		glDrawElements(GL_TRIANGLES, (GLsizei)this->Indices.size(), GL_UNSIGNED_INT, 0);

		if (this->EnableTextureFill) {
			for (int i = 0; i < this->Texture.size(); i++) {
				if (this->Texture[i] == nullptr) {
					continue;
				}
				this->Texture[i]->Unbind();
			}
		}
	}
	
	void Object::AddPosition(float x, float y, float z) {
		this->Position.push_back(x);
		this->Position.push_back(y);
		this->Position.push_back(z);

		this->Vertices.push_back(x);
		this->Vertices.push_back(y);
		this->Vertices.push_back(z);
	}

	void Object::AddNormal(float nx, float ny, float nz) {
		this->Normal.push_back(nx);
		this->Normal.push_back(ny);
		this->Normal.push_back(nz);

		this->Vertices.push_back(nx);
		this->Vertices.push_back(ny);
		this->Vertices.push_back(nz);
	}

	void Object::AddTexCoord(float u, float v) {
		this->TexCoord.push_back(u);
		this->TexCoord.push_back(v);

		this->Vertices.push_back(u);
		this->Vertices.push_back(v);
	}

	void Object::AddIndices(unsigned int i1, unsigned int i2, unsigned int i3) {
		this->Indices.push_back(i1);
		this->Indices.push_back(i2);
		this->Indices.push_back(i3);
	}
}