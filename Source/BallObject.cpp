#include "BallObject.h"
#include <iostream>

BallObject::BallObject() : GameObject(), Radius(0.5f), Elasticities(1.0f) {
    this->Ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0);
    this->Diffuse = glm::vec4(0.25f, 0.25f, 1.0f, 1.0);
    this->Specular = glm::vec4(0.55f, 0.45f, 0.45f, 1.0);
}

BallObject::BallObject(glm::vec3 position, glm::vec3 velocity, GLfloat radius)
        : GameObject(position, glm::vec3(radius),
                     velocity,glm::vec3(0.0f),
                     glm::vec3(0.0f), radius * 2), Radius(radius), Elasticities(1.0f) {
    this->Ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0);
    this->Diffuse = glm::vec4(0.25f, 0.25f, 1.0f, 1.0);
    this->Specular = glm::vec4(0.55f, 0.45f, 0.45f, 1.0);
}

void BallObject::Update(GLfloat dt) {
    if (!this->IsSolid) {
        this->Acceleration = this->Force / this->Mass;
        this->Velocity += this->Acceleration * dt;
        this->Position += this->Velocity * dt;
        this->Force = glm::vec3(0.0f);
    }

    glm::mat4 model(1.0f);
    model = glm::translate(model, this->Position);
    // model = glm::rotate(model, glm::radians(glm::length(this->AngularVelocity)), glm::normalize(this->AngularVelocity));
    model = glm::scale(model, glm::vec3(this->Radius));
    this->Model = model;
}

void BallObject::Edge(GLint x_min, GLint x_max, GLint y_min, GLint y_max, GLint z_min, GLint z_max) {
    if (this->Position.x + this->Radius > x_max) {
        this->Velocity.x = -this->Velocity.x * this->Elasticities;
        this->Position.x = x_max - this->Radius;
    } else if (this->Position.x - Radius < x_min) {
        this->Velocity.x = -this->Velocity.x * this->Elasticities;
        this->Position.x = x_min + this->Radius;
    }

    if (this->Position.y + this->Radius > y_max) {
        this->Velocity.y = -this->Velocity.y * this->Elasticities;
        this->Position.y = y_max - this->Radius;
    } else if (this->Position.y - Radius < y_min) {
        this->Velocity.y = -this->Velocity.y * this->Elasticities;
        this->Position.y = y_min + this->Radius;
    }

    if (this->Position.z + this->Radius > z_max) {
        this->Velocity.z = -this->Velocity.z * this->Elasticities;
        this->Position.z = z_max - this->Radius;
    } else if (this->Position.z - Radius < z_min) {
        this->Velocity.z = -this->Velocity.z * this->Elasticities;
        this->Position.z = z_min + this->Radius;
    }
}


