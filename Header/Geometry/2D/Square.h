#pragma once

#include <iostream>

#include "Object.h"
#include "Shader.h"

namespace Nexus {
	class Square : public Object {
	public:

		Square() {
			this->ShapeName = "Square";
			this->Initialize();
		}

	private:
		void GenerateVertices() override {
			this->AddPosition(-0.5f, -0.5f, 0.0f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, 0.5f, 0.0f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(0.5f, 0.5f, 0.0f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, 0.0f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(1.0f, 0.0f);

			this->AddIndices(0, 1, 2);
			this->AddIndices(0, 2, 3);
		}
	};
}
