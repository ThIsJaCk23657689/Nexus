#pragma once

#include "Object.h"

const unsigned int MIN_LONGITUDE = 3;
const unsigned int MIN_LATITUDE = 1;

namespace Nexus {
	class Cylinder : public Object {
	public:

		Cylinder(float base_radius = 1.0f, float top_radius = 1.0f, float height = 1.0f, unsigned int longitude = 30, unsigned int latitude = 30)
		: BaseRadius(base_radius), TopRadius(top_radius), Height(height) {

			this->ShapeName = "Cylinder";

			this->Longitude = (longitude < MIN_LONGITUDE) ? MIN_LONGITUDE : longitude ;
			this->Latitude = (latitude < MIN_LATITUDE) ? MIN_LATITUDE : latitude;
			
			this->Initialize();
		}

	private:
		float BaseRadius;
		float TopRadius;
		float Height;
		unsigned int Longitude;
		unsigned int Latitude;
		
		unsigned int BaseIndex;
		unsigned int TopIndex;
		std::vector<float> UnitCircleVertices;
		
		void GenerateVertices() override {
			this->GenerateUnitCircleVertices();

			float x, y, z;
			float radius;

			std::vector<float> sideNormals = this->GenerateSideNormals();

			for (unsigned int i = 0; i <= this->Latitude; i++) {
				z = -(this->Height * 0.5f) + (float)i / this->Latitude * this->Height;
				radius = this->BaseRadius + (float)i / this->Latitude * (this->TopRadius - this->BaseRadius);
				float t = 1.0f - (float)i / this->Latitude;

                // 0 ~ this->Longitude 個點
				for (unsigned int j = 0, k = 0; j <= this->Longitude; j++, k += 3) {
					x = this->UnitCircleVertices[k];
					y = this->UnitCircleVertices[k + 1];
					this->AddPosition(x * radius, y * radius, z);
					this->AddNormal(sideNormals[k], sideNormals[k + 1], sideNormals[k + 2]);
					this->AddTexCoord((float)j / this->Longitude, t);
				}
			}

			unsigned int baseVertexIndex = (unsigned int)this->Vertices.size() / 8;
			z = -this->Height * 0.5f;
			this->AddPosition(0, 0, z);
			this->AddNormal(0, 0, -1);
			this->AddTexCoord(0.5f, 0.5f);
			for (unsigned int i = 0, j = 0; i < this->Longitude; i++, j += 3) {
				x = this->UnitCircleVertices[j];
				y = this->UnitCircleVertices[j + 1];
				this->AddPosition(x * this->BaseRadius, y * this->BaseRadius, z);
				this->AddNormal(0, 0, -1);
				this->AddTexCoord(-x * 0.5f + 0.5f, -y * 0.5f + 0.5f);
			}

			unsigned int topVertexIndex = (unsigned int)this->Vertices.size() / 8;
			z = this->Height * 0.5f;
			this->AddPosition(0, 0, z);
			this->AddNormal(0, 0, 1);
			this->AddTexCoord(0.5f, 0.5f);
			for (unsigned int i = 0, j = 0; i < this->Longitude; i++, j += 3) {
				x = this->UnitCircleVertices[j];
				y = this->UnitCircleVertices[j + 1];
				this->AddPosition(x * this->TopRadius, y * this->TopRadius, z);
				this->AddNormal(0, 0, 1);
				this->AddTexCoord(x * 0.5f + 0.5f, -y * 0.5f + 0.5f);
			}

			unsigned int k1, k2;
			for (unsigned int i = 0; i < this->Latitude; i++) {
				k1 = i * (this->Longitude + 1);
				k2 = k1 + this->Longitude + 1;
				for (unsigned int j = 0; j < this->Longitude; j++, k1++, k2++) {
					this->AddIndices(k1, k2, k1 + 1);
					this->AddIndices(k2, k2 + 1, k1 + 1);
				}
			}

			this->BaseIndex = (unsigned int)this->Indices.size();
			for (unsigned int i = 0, k = baseVertexIndex + 1; i < this->Longitude; i++, k++) {
				if (i < (this->Longitude - 1)) {
					this->AddIndices(baseVertexIndex, k + 1, k);
				} else {
					this->AddIndices(baseVertexIndex, baseVertexIndex + 1, k);
				}
			}

			this->TopIndex = (unsigned int)this->Indices.size();
			for (unsigned int i = 0, k = topVertexIndex + 1; i < this->Longitude; i++, k++) {
				if (i < (this->Longitude - 1)) {
					this->AddIndices(topVertexIndex, k, k + 1);
				} else {
					this->AddIndices(topVertexIndex, k, topVertexIndex + 1);
				}
			}
		}

		void GenerateUnitCircleVertices() {
			float sectorStep = 2 * PI / this->Longitude;
			float sectorAngle;

			for (unsigned int i = 0; i <= this->Longitude; i++) {
				sectorAngle = i * sectorStep;
				this->UnitCircleVertices.push_back(cos(sectorAngle));
				this->UnitCircleVertices.push_back(sin(sectorAngle));
				this->UnitCircleVertices.push_back(0);
			}
		}

		std::vector<float> GenerateSideNormals() {
			std::vector<float> sideNormal;
			float sectorStep = PI * 2 / this->Longitude;
			float sectorAngle;

			// Compute the normal vector at 0 degree first
			float zAngle = atan2(this->BaseRadius - this->TopRadius, this->Height);
			float x0 = cos(zAngle);
			float y0 = 0;
			float z0 = sin(zAngle);

			std::vector<float> normals;
			for (unsigned int i = 0; i <= this->Longitude; i++) {
				sectorAngle = i * sectorStep;
				sideNormal.push_back(cos(sectorAngle) * x0 - sin(sectorAngle) * y0);
				sideNormal.push_back(sin(sectorAngle) * x0 + cos(sectorAngle) * y0);
				sideNormal.push_back(z0);
			}

			return sideNormal;
		}

	};
}
