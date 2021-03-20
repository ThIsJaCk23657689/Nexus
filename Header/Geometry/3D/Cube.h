#pragma once

#include <iostream>

#include "Logger.h"
#include "Object.h"
#include "Shader.h"

namespace Nexus {
	class Cube : public Object {
	public:

		Cube() {
			this->VertexCount = 36;

			// ========== Front ==========
			this->AddPosition(-0.5f, -0.5f, 0.5f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, 0.5f, 0.5f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 0.1f);

			this->AddPosition(0.5f, 0.5f, 0.5f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, 0.5f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Top ==========
			this->AddPosition(-0.5f, 0.5f, 0.5f);
			this->AddNormal(0.0f, 1.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, 0.5f, -0.5f);
			this->AddNormal(0.0f, 1.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.1f);

			this->AddPosition(0.5f, 0.5f, -0.5f);
			this->AddNormal(0.0f, 1.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, 0.5f, 0.5f);
			this->AddNormal(0.0f, 1.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Right ==========
			this->AddPosition(0.5f, -0.5f, 0.5f);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(0.5f, 0.5f, 0.5f);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.1f);

			this->AddPosition(0.5f, 0.5f, -0.5f);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, -0.5f);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Back ==========
			this->AddPosition(-0.5f, -0.5f, -0.5f);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, 0.5f, -0.5f);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(0.0f, 0.1f);

			this->AddPosition(0.5f, 0.5f, -0.5f);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, -0.5f);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Bottom ==========
			this->AddPosition(-0.5f, -0.5f, 0.5f);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, -0.5f, -0.5f);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.1f);

			this->AddPosition(0.5f, -0.5f, -0.5f);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, 0.5f);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Left ==========
			this->AddPosition(-0.5f, -0.5f, 0.5f);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, 0.5f, 0.5f);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.1f);

			this->AddPosition(-0.5f, 0.5f, -0.5f);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(-0.5f, -0.5f, -0.5f);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Indices ==========
			this->AddIndices(0, 1, 2);
			this->AddIndices(0, 2, 3);
			this->AddIndices(4, 5, 6);
			this->AddIndices(4, 6, 7);
			this->AddIndices(8, 9, 10);
			this->AddIndices(8, 10, 11);
			this->AddIndices(12, 13, 14);
			this->AddIndices(12, 14, 15);
			this->AddIndices(16, 17, 18);
			this->AddIndices(16, 18, 19);
			this->AddIndices(20, 21, 22);
			this->AddIndices(20, 22, 23);

			Initialize();
		}

		void Initialize() override {

			this->VBO = std::make_unique<Nexus::VertexBuffer>(this->Vertices.data(), this->GetVertexCount() * sizeof(Vertex));
			this->EBO = std::make_unique<Nexus::IndexBuffer>(this->Indices.data(), this->Indices.size() * sizeof(unsigned int));

			Nexus::VertexAttributes Attribs[] = { {3, 0}, {3, offsetof(Vertex, Normal)}, {2, offsetof(Vertex, TexCoord)} };
			this->VAO = std::make_unique<Nexus::VertexArray>(this->VBO.get(), Attribs, 3, (GLsizei)sizeof(Vertex), this->EBO.get());
		}

		void Debug() override {
			std::cout << "===== Square =====\n"
				<< "Vertex Count: " << GetVertexCount() << std::endl
				<< "Position Count: " << GetPositionCount() << std::endl
				<< "Normal Count: " << GetNormalCount() << std::endl
				<< "TexCoord Count: " << GetTexCoordCount() << std::endl
				<< "Index Count: " << GetIndexCount() << std::endl;;
		}

		void Draw(Nexus::Shader* shader) override {

			shader->Use();
			shader->SetVec3("color", this->Color);

			this->VAO->Bind();
			if (this->EnableWireFrameMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}

			glDrawElements(GL_TRIANGLES, (GLsizei)this->Indices.size(), GL_UNSIGNED_INT, 0);
		}
	};
}
