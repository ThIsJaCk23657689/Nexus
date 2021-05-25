#pragma once

#include "Object.h"

namespace Nexus {
	class NDCQuad : public Object {
	public:
		
		NDCQuad() {
			this->ShapeName = "NDC Quad";
			this->Initialize();
		}

		void Draw(Nexus::Shader* shader) {
			shader->Use();

			this->VAO->Bind();
			if (this->EnableWireFrameMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			glDrawElements(GL_TRIANGLES, (GLsizei)this->Indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		
	private:
		struct QuadVertex {
			glm::vec2 Position;
			glm::vec2 TexCoord;
		};
		
		void GenerateVertices() override {
			this->AddPosition(-1.0f, -1.0f);
			this->AddTexCoord( 0.0f,  0.0f);

			this->AddPosition( 1.0f, -1.0f);
			this->AddTexCoord( 1.0f,  0.0f);

			this->AddPosition( 1.0f,  1.0f);
			this->AddTexCoord( 1.0f,  1.0f);

			this->AddPosition(-1.0f,  1.0f);
			this->AddTexCoord( 0.0f,  1.0f);

			this->AddIndices(0, 1, 2);
			this->AddIndices(0, 2, 3);
		}

		void BufferInitialize() override {
			this->VBO = std::make_unique<Nexus::VertexBuffer>(this->Vertices.data(), (this->Vertices.size() / 4.0f) * sizeof(QuadVertex));
			this->EBO = std::make_unique<Nexus::IndexBuffer>(this->Indices.data(), this->Indices.size() * sizeof(unsigned int));

			Nexus::VertexAttributes Attribs[] = { {2, 0}, {2, offsetof(QuadVertex, TexCoord)} };
			this->VAO = std::make_unique<Nexus::VertexArray>(this->VBO.get(), Attribs, 2, (GLsizei)sizeof(QuadVertex), this->EBO.get());
		}
	};
}