#pragma once

#include "Object.h"

namespace Nexus {
	class Triangle : public Object {
	public:

		Triangle() {

			this->ShapeName = "Triangle";
			this->VertexCount = 3;
			
			// Setting Data
			this->AddPosition(-0.5f, -0.5f, 0.0f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(0.0f, 0.5f, 0.0f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.5f, 1.0f);

			this->AddPosition(0.5f, -0.5f, 0.0f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(1.0f, 0.0f);

			this->AddIndices(0, 1, 2);

			Initialize();
		}

		void Initialize() override {
			this->VBO = std::make_unique<Nexus::VertexBuffer>(this->Vertices.data(), this->GetVertexCount() * sizeof(Vertex));
			this->EBO = std::make_unique<Nexus::IndexBuffer>(this->Indices.data(), this->Indices.size() * sizeof(unsigned int));

			Nexus::VertexAttributes Attribs[] = { {3, 0}, {3, offsetof(Vertex, Normal)}, {2, offsetof(Vertex, TexCoord)} };
			this->VAO = std::make_unique<Nexus::VertexArray>(this->VBO.get(), Attribs, 3, (GLsizei)sizeof(Vertex), this->EBO.get());
		}
	};
}
