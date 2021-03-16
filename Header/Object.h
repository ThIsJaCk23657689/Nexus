#pragma once

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"

#include <vector>
#include <memory>

namespace Nexus {

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};
	
	class Object {
	public:

		virtual void Initialize() = 0;
		virtual void Draw(Nexus::Shader* shader) = 0;
		
		virtual ~Object() {}

		unsigned int getVertexCount() const { return (unsigned int)this->Vertices.size() / 8; }
		unsigned int getPositionCount() const { return (unsigned int)this->Position.size() / 3; }
		unsigned int getNormalCount() const { return (unsigned int)this->Normal.size() / 3; }
		unsigned int getTexCoordCount() const { return (unsigned int)this->TexCoord.size() / 2; }
		unsigned int getIndexCount() const { return (unsigned int)this->Indices.size(); }
		
	protected:
		std::vector<float> Vertices;
		std::vector<float> Position;
		std::vector<float> Normal;
		std::vector<float> TexCoord;
		std::vector<GLuint> Indices;

		std::unique_ptr<Nexus::VertexArray> VAO;
		std::unique_ptr<Nexus::VertexBuffer> VBO;
		std::unique_ptr<Nexus::IndexBuffer> EBO;

		void addPosition(float x, float y, float z);
		void addNormal(float nx, float ny, float nz);
		void addTexCoord(float u, float v);
		void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);
		
	};
}