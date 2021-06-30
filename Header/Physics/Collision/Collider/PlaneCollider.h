#pragma once
#include <glm/glm.hpp>
#include "Physics/Collision.h"
#include "Physics/Collision/Collider/Collider.h"
#include "Physics/Collision/Collider/SphereCollider.h"

namespace Nexus {
    struct PlaneCollider : Collider {
        glm::vec3 Plane;
        float Distance;

        CollisionPoints TestCollision (
                const Transform* transform,
                const Collider* collider,
                const Transform* collider_transform) const override {
            return collider->TestCollision(collider_transform, this, transform);
        }

        CollisionPoints TestCollision (
                const Transform* transform,
                const SphereCollider* sphere,
                const Transform* sphere_transform) const override {
            return sphere->TestCollision(sphere_transform, this, transform);
        }

        CollisionPoints TestCollision (
                const Transform* transform,
                const PlaneCollider* plane,
                const Transform* plane_transform) const override {
            return {};
        }
    };
}
