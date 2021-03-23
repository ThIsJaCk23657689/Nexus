#pragma once

#include "Object.h"

namespace Nexus {
	class Triangle : public Object {
	public:

		Triangle() {
			this->ShapeName = "Triangle";
			this->Initialize();
		}
		
	private:
		void GenerateVertices() override {
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
		}
	};
}
