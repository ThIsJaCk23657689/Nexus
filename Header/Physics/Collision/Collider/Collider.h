#pragma once
#include <glm/glm.hpp>
#include "Physics/Collision.h"
#include "Physics/Collision/Collider/SphereCollider.h"
#include "Physics/Collision/Collider/PlaneCollider.h"

namespace Nexus {
    struct Collider {
        virtual CollisionPoints TestCollision (
                const Transform* transform,
                const Collider* collider,
                const Transform* collider_transform) const = 0;

        virtual CollisionPoints TestCollision (
                const Transform* transform,
                const SphereCollider* sphere,
                const Transform* sphere_transform) const = 0;

        virtual CollisionPoints TestCollision (
                const Transform* transform,
                const PlaneCollider* plane,
                const Transform* plane_transform) const = 0;
    };
}