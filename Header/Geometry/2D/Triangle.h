#pragma once

#include <iostream>

#include "Object.h"
#include "Shader.h"

#define VERTEXATTRCOUNT 8

namespace Nexus {
	class Triangle : public Object {
	public:

		Triangle() {

			// Setting Data
			this->addPosition(-0.5f, -0.5f, 0.0f);
			this->addNormal(0.0f, 0.0f, 1.0f);
			this->addTexCoord(0.0f, 0.0f);

			this->addPosition(0.5f, -0.5f, 0.0f);
			this->addNormal(0.0f, 0.0f, 1.0f);
			this->addTexCoord(1.0f, 0.0f);

			this->addPosition(0.0f, 0.5f, 0.0f);
			this->addNormal(0.0f, 0.0f, 1.0f);
			this->addTexCoord(0.5f, 1.0f);
			
			Initialize();
			
		}

		void Initialize() {

			this->VBO = std::make_unique<Nexus::VertexBuffer>(this->Vertices.data(), this->Vertices.size() * sizeof(Vertex));
			// this->EBO = std::make_unique<Nexus::IndexBuffer>(indices.data(), indices.size() * sizeof(unsigned int));
			
			Nexus::VertexAttributes Attribs[] = { {3, 0}, {3, offsetof(Vertex, Normal)}, {2, offsetof(Vertex, TexCoord)} };
			this->VAO = std::make_unique<Nexus::VertexArray>(this->VBO.get(), Attribs, 3, (GLsizei)sizeof(Vertex));

			/*
			glGenVertexArrays(1, &triangleVAO);
			glGenBuffers(1, &triangleVBO);
			glBindVertexArray(triangleVAO);
			glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
			glBufferData(GL_ARRAY_BUFFER, triangleVertices.size() * sizeof(float), triangleVertices.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glBindVertexArray(0);
			*/
		}

		void Debug() {
			std::cout << "===== Triangle =====\n"
				<< "Vertex Count: " << getVertexCount() << std::endl
				<< "Position Count: " << getPositionCount() << std::endl
				<< "Normal Count: " << getNormalCount() << std::endl
				<< "TexCoord Count: " << getTexCoordCount() << std::endl;
		}
		
		void Draw(Nexus::Shader* shader) {
			shader->Use();
			this->VAO->Bind();
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		
	};
}
