#pragma once

#include "Object.h"

namespace Nexus {
	void Object::addPosition(float x, float y, float z) {
		this->Position.push_back(x);
		this->Position.push_back(y);
		this->Position.push_back(z);

		this->Vertices.push_back(x);
		this->Vertices.push_back(y);
		this->Vertices.push_back(z);
		// std::cout << "x:" << x << ", y:" << y << ", z:" << z << std::endl;
	}

	void Object::addNormal(float nx, float ny, float nz) {
		this->Normal.push_back(nx);
		this->Normal.push_back(ny);
		this->Normal.push_back(nz);

		this->Vertices.push_back(nx);
		this->Vertices.push_back(ny);
		this->Vertices.push_back(nz);
	}

	void Object::addTexCoord(float u, float v) {
		this->TexCoord.push_back(u);
		this->TexCoord.push_back(v);

		this->Vertices.push_back(u);
		this->Vertices.push_back(v);
	}

	void Object::addIndices(unsigned int i1, unsigned int i2, unsigned int i3) {
		this->Indices.push_back(i1);
		this->Indices.push_back(i2);
		this->Indices.push_back(i3);
		// std::cout << "i1:" << i1 << ", i2:" << i2 << ", i3:" << i3 << std::endl;
	}
}