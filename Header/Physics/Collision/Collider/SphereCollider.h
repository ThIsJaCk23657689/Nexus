#pragma once
#include <glm/glm.hpp>
#include "Physics/Collision.h"
#include "Physics/Collision/Collider/Collider.h"
#include "Physics/Collision/Collider/PlaneCollider.h"

namespace Nexus {
    struct SphereCollider : Collider {
        glm::vec3 Center;
        float Radius;

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
            return algo::FindSphereSphereCollisonPoints(this, transform, sphere, sphere_transform);
        }

        CollisionPoints TestCollision (
                const Transform* transform,
                const PlaneCollider* plane,
                const Transform* plane_transform) const override {
            return algo::FindSpherePlaneCollisonPoints(this, transform, plane, plane_transform);
        }
    };
}