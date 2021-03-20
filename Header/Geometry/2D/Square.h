#pragma once

#include <iostream>

#include "Object.h"
#include "Shader.h"

namespace Nexus {
	class Square : public Object {
	public:

		Square() {
			this->VertexCount = 4;
			
			// Setting Data
			this->AddPosition(-0.5f, -0.5f, 0.0f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, 0.5f, 0.0f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 0.1f);

			this->AddPosition(0.5f, 0.5f, 0.0f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, 0.0f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(1.0f, 0.0f);

			this->AddIndices(0, 1, 2);
			this->AddIndices(0, 2, 3);

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
				<< "Index Count: " << GetIndexCount() << std::endl;
		}

		void Draw(Nexus::Shader* shader) override {

			shader->Use();
			shader->SetVec3("color", this->Color);
			
			this->VAO->Bind();
			if(this->EnableWireFrameMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			
			glDrawElements(GL_TRIANGLES, (GLsizei)this->Indices.size(), GL_UNSIGNED_INT, 0);
		}
	};
}
