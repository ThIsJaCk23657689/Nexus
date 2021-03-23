#pragma once

#include "Object.h"

namespace Nexus {

	enum Orientation {
		POS_X,
		POS_Y,
		POS_Z,
	};
	
	class Rectangle : public Object {
	public:

		Rectangle() {
			this->ShapeName = "Rectangle";
			this->Initialize();
		}

		Rectangle(float width, float height, float texture_scale, Orientation direction = POS_Z) :
			Width(width), Height(height), TextureScale(texture_scale), Direction(direction) {
			this->ShapeName = "Rectangle";
			this->Initialize();
		}

		Rectangle(float size, Orientation direction = POS_Z) : Direction(direction) {
			this->ShapeName = "Rectangle";
			this->Width = size;
			this->Height = size;
			this->Initialize();
		}

		Rectangle(Orientation direction) : Direction(direction) {
			this->ShapeName = "Rectangle";
			this->Initialize();
		}

	private:
		float Width = 1.0f;
		float Height = 1.0f;
		float TextureScale = 1.0f;
		Orientation Direction = POS_Z;
		
		void GenerateVertices() override {

			float w = this->Width / 2.0f;
			float h = this->Height / 2.0f;
			
			switch (this->Direction) {
				case POS_X:
					this->AddPosition(0.0f, w, -h);
					this->AddNormal(1.0f, 0.0f, 0.0f);
					this->AddTexCoord(0.0f, 0.0f);

					this->AddPosition(0.0f, w, h);
					this->AddNormal(1.0f, 0.0f, 0.0f);
					this->AddTexCoord(0.0f, 1.0f * this->TextureScale);

					this->AddPosition(0.0f, -w, h);
					this->AddNormal(1.0f, 0.0f, 0.0f);
					this->AddTexCoord(1.0f * this->TextureScale, 1.0f * this->TextureScale);

					this->AddPosition(0.0f, -w, -h);
					this->AddNormal(1.0f, 0.0f, 0.0f);
					this->AddTexCoord(1.0f * this->TextureScale, 0.0f);
					break;
				
				case POS_Y:
					this->AddPosition(-w, 0.0f, h);
					this->AddNormal(0.0f, 1.0f, 0.0f);
					this->AddTexCoord(0.0f, 0.0f);

					this->AddPosition(-w, 0.0f, -h);
					this->AddNormal(0.0f, 1.0f, 0.0f);
					this->AddTexCoord(0.0f, 1.0f * this->TextureScale);

					this->AddPosition(+w, 0.0f, -h);
					this->AddNormal(0.0f, 1.0f, 0.0f);
					this->AddTexCoord(1.0f * this->TextureScale, 1.0f * this->TextureScale);

					this->AddPosition(w, 0.0f, h);
					this->AddNormal(0.0f, 1.0f, 0.0f);
					this->AddTexCoord(1.0f * this->TextureScale, 0.0f);
					break;
				
				case POS_Z:
					this->AddPosition(-w, -h, 0.0f);
					this->AddNormal(0.0f, 0.0f, 1.0f);
					this->AddTexCoord(0.0f, 0.0f);

					this->AddPosition(-w, h, 0.0f);
					this->AddNormal(0.0f, 0.0f, 1.0f);
					this->AddTexCoord(0.0f, 1.0f * this->TextureScale);

					this->AddPosition(w, h, 0.0f);
					this->AddNormal(0.0f, 0.0f, 1.0f);
					this->AddTexCoord(1.0f * this->TextureScale, 1.0f * this->TextureScale);

					this->AddPosition(w, -h, 0.0f);
					this->AddNormal(0.0f, 0.0f, 1.0f);
					this->AddTexCoord(1.0f * this->TextureScale, 0.0f);
					break;
			}
			
			this->AddIndices(0, 1, 2);
			this->AddIndices(0, 2, 3);
		}
	};
}
