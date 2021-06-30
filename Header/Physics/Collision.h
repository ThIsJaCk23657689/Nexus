#pragma once
#include <glm/glm.hpp>

namespace Nexus {
    struct CollisionPoints {
        glm::vec3 A;
        glm::vec3 B;
        glm::vec3 Normal;
        float Depth;
        bool HasCollision;
    };

    struct Transform {
        glm::vec3 Position;
        glm::vec3 Scale;
        glm::vec3 RotationAxis;
        float RotationAngle;
    };
}