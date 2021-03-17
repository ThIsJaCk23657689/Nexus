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
			this->addPosition(-0.5f, -0.5f, 0.0f);
			this->addNormal(0.0f, 0.0f, 1.0f);
			this->addTexCoord(0.0f, 0.0f);

			this->addPosition(-0.5f, 0.5f, 0.0f);
			this->addNormal(0.0f, 0.0f, 1.0f);
			this->addTexCoord(0.0f, 0.1f);

			this->addPosition(0.5f, 0.5f, 0.0f);
			this->addNormal(0.0f, 0.0f, 1.0f);
			this->addTexCoord(1.0f, 1.0f);

			this->addPosition(0.5f, -0.5f, 0.0f);
			this->addNormal(0.0f, 0.0f, 1.0f);
			this->addTexCoord(1.0f, 0.0f);

			this->addIndices(0, 1, 2);
			this->addIndices(0, 2, 3);

			Initialize();
		}

		void Initialize() override {
			this->VBO = std::make_unique<Nexus::VertexBuffer>(this->Vertices.data(), this->Vertices.size() * sizeof(Vertex));
			this->EBO = std::make_unique<Nexus::IndexBuffer>(this->Indices.data(), this->Indices.size() * sizeof(unsigned int));

			Nexus::VertexAttributes Attribs[] = { {3, 0}, {3, offsetof(Vertex, Normal)}, {2, offsetof(Vertex, TexCoord)} };
			this->VAO = std::make_unique<Nexus::VertexArray>(this->VBO.get(), Attribs, 3, (GLsizei)sizeof(Vertex), this->EBO.get());
		}

		void Debug() override {
			std::cout << "===== Square =====\n"
				<< "Vertex Count: " << getVertexCount() << std::endl
				<< "Position Count: " << getPositionCount() << std::endl
				<< "Normal Count: " << getNormalCount() << std::endl
				<< "TexCoord Count: " << getTexCoordCount() << std::endl
				<< "Index Count: " << getIndexCount() << std::endl;;
		}

		void Draw(Nexus::Shader* shader) override {

			shader->Use();
			shader->SetVec3("Color", this->Color);
			
			this->VAO->Bind();
			if(this->EnableWireFrameMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			
			glDrawElements(GL_TRIANGLES, (GLsizei)this->Indices.size(), GL_UNSIGNED_INT, 0);
		}
	};
}
