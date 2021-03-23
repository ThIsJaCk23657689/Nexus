#pragma once

#include "Object.h"

namespace Nexus {
	class Sphere : public Object {
	public:
		
		Sphere(float radius = 1.0f, unsigned int longitude = 30.0f, unsigned int latitude = 30.0f) : Radius(radius), Longitude(longitude), Latitude(latitude) {
			this->ShapeName = "Sphere";
			this->Initialize();
		}

	private:
		float Radius;
		unsigned int Longitude;
		unsigned int Latitude;

		void GenerateVertices() override {
			for (int i = 0; i <= this->Latitude; i++) {
				float theta = i * PI / this->Latitude;
				float sinTheta = sin(theta);
				float cosTheta = cos(theta);
				for (int j = 0; j <= this->Longitude; j++) {
					float phi = j * 2.0f * PI / this->Longitude;
					float sinPhi = sin(phi);
					float cosPhi = cos(phi);

					float x = cosPhi * sinTheta;
					float y = cosTheta;
					float z = sinPhi * sinTheta;

					this->AddPosition(this->Radius * x, this->Radius * y, this->Radius * z);

					// Generate normal vectors
					glm::vec3 normal = glm::vec3(2 * this->Radius * x, 2 * this->Radius * y, 2 * this->Radius * z);
					normal = glm::normalize(normal);
					this->AddNormal(normal.x, normal.y, normal.z);

					// Generate texture coordinate
					float u = 1 - (j / this->Longitude);
					float v = 1 - (i / this->Latitude);
					this->AddTexCoord(u, -v);

					this->VertexCount++;
				}
			}

			for (int i = 0; i < this->Latitude; i++) {
				for (int j = 0; j < this->Longitude; j++) {
					int first = (i * (this->Longitude + 1)) + j;
					int second = first + this->Longitude + 1;

					this->AddIndices(first, second, first + 1);
					this->AddIndices(second, second + 1, first + 1);
				}
			}
		}
		
	};
}
