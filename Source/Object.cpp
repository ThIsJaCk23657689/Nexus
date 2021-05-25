#include "Object.h"

#include "Logger.h"

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

	void Object::Draw(Nexus::Shader* shader, const glm::mat4& model) {
		shader->Use();
		shader->SetMat3("normalModel", glm::mat3(glm::transpose(glm::inverse(model))));
		shader->SetMat4("model", model);
		
		this->VAO->Bind();
		if (this->EnableWireFrameMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glDrawElements(GL_TRIANGLES, (GLsizei)this->Indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void Object::DrawInstancing(Nexus::Shader* shader, std::vector<glm::mat4>& models) {
		shader->Use();
		
		this->VAO->Bind();
		
		unsigned int buffer;
		GLsizei vec4Size = sizeof(glm::vec4);
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		
		glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)this->Indices.size(), GL_UNSIGNED_INT, 0, 1000);
		glBindVertexArray(0);
	}

	void Object::BufferInitialize() {
		this->VBO = std::make_unique<Nexus::VertexBuffer>(this->Vertices.data(), this->GetVertexCount() * sizeof(Vertex));
		this->EBO = std::make_unique<Nexus::IndexBuffer>(this->Indices.data(), this->Indices.size() * sizeof(unsigned int));

		Nexus::VertexAttributes Attribs[] = { {3, 0}, {3, offsetof(Vertex, Normal)}, {2, offsetof(Vertex, TexCoord)} };
		this->VAO = std::make_unique<Nexus::VertexArray>(this->VBO.get(), Attribs, 3, (GLsizei)sizeof(Vertex), this->EBO.get());
	}

	void Object::AddPosition(float x, float y) {
		this->Position.push_back(x);
		this->Position.push_back(y);

		this->Vertices.push_back(x);
		this->Vertices.push_back(y);
	}
	
	void Object::AddPosition(float x, float y, float z) {
		this->Position.push_back(x);
		this->Position.push_back(y);
		this->Position.push_back(z);

		this->Vertices.push_back(x);
		this->Vertices.push_back(y);
		this->Vertices.push_back(z);
	}

	void Object::AddPosition(glm::vec4 position) {
		this->Position.push_back(position.x);
		this->Position.push_back(position.y);
		this->Position.push_back(position.z);

		this->Vertices.push_back(position.x);
		this->Vertices.push_back(position.y);
		this->Vertices.push_back(position.z);
	}

	void Object::AddNormal(float nx, float ny, float nz) {
		this->Normal.push_back(nx);
		this->Normal.push_back(ny);
		this->Normal.push_back(nz);

		this->Vertices.push_back(nx);
		this->Vertices.push_back(ny);
		this->Vertices.push_back(nz);
	}

	void Object::AddNormal(glm::vec3 normal) {
		this->Normal.push_back(normal.x);
		this->Normal.push_back(normal.y);
		this->Normal.push_back(normal.z);

		this->Vertices.push_back(normal.x);
		this->Vertices.push_back(normal.y);
		this->Vertices.push_back(normal.z);
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
