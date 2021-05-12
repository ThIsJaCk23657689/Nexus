#pragma once

#include <iostream>
#include "Object.h"

namespace Nexus {
	class Cube : public Object {
	public:

		Cube() {
			this->ShapeName = "Cube";
			this->Initialize();
		}

	private:
		
		void GenerateVertices() override {
			// ========== Front ==========
			this->AddPosition(-0.5f, -0.5f, 0.5f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, 0.5f, 0.5f);
			this->AddNormal(0.0f, 0.0f, 1.0f);
			this->AddTexCoord(0.0f, 1.0f);

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
			this->AddTexCoord(0.0f, 1.0f);

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
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(0.5f, 0.5f, -0.5f);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, -0.5f);
			this->AddNormal(1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Back ==========
			this->AddPosition(0.5f, -0.5f, -0.5f);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(0.5f, 0.5f, -0.5f);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(-0.5f, 0.5f, -0.5f);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(-0.5f, -0.5f, -0.5f);
			this->AddNormal(0.0f, 0.0f, -1.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Bottom ==========
			this->AddPosition(-0.5f, -0.5f, -0.5f);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, -0.5f, 0.5f);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, 0.5f);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(0.5f, -0.5f, -0.5f);
			this->AddNormal(0.0f, -1.0f, 0.0f);
			this->AddTexCoord(1.0f, 0.0f);

			// ========== Left ==========
			this->AddPosition(-0.5f, -0.5f, -0.5f);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 0.0f);

			this->AddPosition(-0.5f, 0.5f, -0.5f);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(0.0f, 1.0f);

			this->AddPosition(-0.5f, 0.5f, 0.5f);
			this->AddNormal(-1.0f, 0.0f, 0.0f);
			this->AddTexCoord(1.0f, 1.0f);

			this->AddPosition(-0.5f, -0.5f, 0.5f);
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
		}
	};
}
