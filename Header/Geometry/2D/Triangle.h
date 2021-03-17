#pragma once

#include <iostream>

#include "Object.h"
#include "Shader.h"

namespace Nexus {
	class Triangle : public Object {
	public:

		Triangle() {

			this->VertexCount = 3;
			
			// Setting Data
			this->addPosition(-0.5f, -0.5f, 0.0f);
			this->addNormal(0.0f, 0.0f, 1.0f);
			this->addTexCoord(0.0f, 0.0f);

			this->addPosition(0.0f, 0.5f, 0.0f);
			this->addNormal(0.0f, 0.0f, 1.0f);
			this->addTexCoord(0.5f, 1.0f);

			this->addPosition(0.5f, -0.5f, 0.0f);
			this->addNormal(0.0f, 0.0f, 1.0f);
			this->addTexCoord(1.0f, 0.0f);

			Initialize();
		}

		void Initialize() override {

			this->VBO = std::make_unique<Nexus::VertexBuffer>(this->Vertices.data(), this->Vertices.size() * sizeof(Vertex));
			// this->EBO = std::make_unique<Nexus::IndexBuffer>(indices.data(), indices.size() * sizeof(unsigned int));
			
			Nexus::VertexAttributes Attribs[] = { {3, 0}, {3, offsetof(Vertex, Normal)}, {2, offsetof(Vertex, TexCoord)} };
			this->VAO = std::make_unique<Nexus::VertexArray>(this->VBO.get(), Attribs, 3, (GLsizei)sizeof(Vertex));
		}

		void Debug() override {
			std::cout << "===== Triangle =====\n"
				<< "Vertex Count: " << getVertexCount() << std::endl
				<< "Position Count: " << getPositionCount() << std::endl
				<< "Normal Count: " << getNormalCount() << std::endl
				<< "TexCoord Count: " << getTexCoordCount() << std::endl;
		}
		
		void Draw(Nexus::Shader* shader) override {
			shader->Use();
			this->VAO->Bind();
			glDrawArrays(GL_TRIANGLES, 0, this->VertexCount);
		}
		
	};
}