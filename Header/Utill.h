#pragma once

#include <glm/glm.hpp>

namespace Nexus {
	class Utill {
	public:
		static glm::vec3 limit(glm::vec3 vector, float number) {
			if (glm::length(vector) > number) {
				vector = glm::normalize(vector) * number;
			}
			return vector;
		}

		static glm::vec3 clamp(glm::vec3 vector, float min, float max) {
			if (glm::length(vector) < min) {
				vector = glm::normalize(vector) * min;
			}
			if (glm::length(vector) > max) {
				vector = glm::normalize(vector) * max;
			}
			return vector;
		}
	};
}