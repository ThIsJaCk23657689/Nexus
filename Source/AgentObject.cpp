#include "AgentObject.h"
#include <iostream>
#include <map>

AgentObject::AgentObject() : GameObject(), Radius(0.5f), Elasticities(1.0f) {
    this->Ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0);
    this->Diffuse = glm::vec4(0.25f, 0.25f, 1.0f, 1.0);
    this->Specular = glm::vec4(0.55f, 0.45f, 0.45f, 1.0);
    this->VisualRadius = this->Radius + 3.0f;
    this->FoodAmount = 6;
    this->LastFoodFrame = 0;
}

AgentObject::AgentObject(glm::vec3 position, glm::vec3 velocity, GLfloat radius)
        : GameObject(position, glm::vec3(radius),
                     velocity,glm::vec3(0.0f),
                     glm::vec3(0.0f), radius * 2), Radius(radius), Elasticities(1.0f) {
    this->Ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0);
    this->Diffuse = glm::vec4(0.25f, 0.25f, 1.0f, 1.0);
    this->Specular = glm::vec4(0.55f, 0.45f, 0.45f, 1.0);
    this->VisualRadius = this->Radius + 3.0f;
    this->FoodAmount = 6;
    this->LastFoodFrame = 0;
}

void AgentObject::Update(GLfloat dt) {
    if (!this->IsSolid) {
        this->Acceleration = this->Force / this->Mass;
        this->Acceleration += glm::vec3(0.0f, -9.8f, 0.0f);
        this->Velocity += this->Acceleration * dt;
        this->Position += this->Velocity * dt;

        glm::mat4 model(1.0f);
        model = glm::translate(model, this->Position);
        // model = glm::rotate(model, glm::radians(glm::length(this->AngularVelocity)), glm::normalize(this->AngularVelocity));
        model = glm::scale(model, glm::vec3(this->Radius));
        this->Model = model;

        this->Force = glm::vec3(0.0f);
    }
}

void AgentObject::SearchFood(BallObject food) {
    if (food.Destroyed) {
        return;
    }

    glm::vec3 diff = food.Position - this->Position;
    if (glm::length(diff) < this->VisualRadius) {
        glm::vec3 force = (diff - this->Velocity) * 1.0f / glm::length(diff);
        this->Force += force;
    }
}

void AgentObject::Runaway(AgentObject enemy) {
    GLfloat speed = glm::length(enemy.Velocity);
    GLfloat distance = glm::distance(this->Position, enemy.Position);

    if (distance == 0) {
        this->DistanceMemberFunc["very close"] = 1.0f;
    } else if (distance > 0 && distance < 20) {
        this->DistanceMemberFunc["very close"] = 1 + (distance - 0) * (0 - 1) / (20 - 0);
    } else if (distance >= 20) {
        this->DistanceMemberFunc["very close"] = 0.0f;
    }

    if (distance <= 10) {
        this->DistanceMemberFunc["close"] = 0.0f;
    } else if (distance > 10 && distance < 30) {
        this->DistanceMemberFunc["close"] = 0 + (distance - 10) * (1 - 0) / (30 - 10);
    } else if (distance == 30) {
        this->DistanceMemberFunc["close"] = 1.0f;
    } else if (distance > 30 && distance < 50) {
        this->DistanceMemberFunc["close"] = 1 + (distance - 30) * (0 - 1) / (50 - 30);
    }  else if (distance >= 50) {
        this->DistanceMemberFunc["close"] = 0.0f;
    }

    if (distance <= 30) {
        this->DistanceMemberFunc["far"] = 0.0f;
    } else if (distance > 30 && distance < 50) {
        this->DistanceMemberFunc["far"] = 0 + (distance - 30) * (1 - 0) / (50 - 30);
    } else if (distance == 50) {
        this->DistanceMemberFunc["far"] = 1.0f;
    } else if (distance > 50 && distance < 70) {
        this->DistanceMemberFunc["far"] = 1 + (distance - 50) * (0 - 1) / (70 - 50);
    }  else if (distance >= 70) {
        this->DistanceMemberFunc["far"] = 0.0f;
    }

    if (distance <= 60) {
        this->DistanceMemberFunc["very far"] = 0.0f;
    } else if (distance > 60 && distance < 70) {
        this->DistanceMemberFunc["very far"] = 0 + (distance - 60) * (1 - 0) / (70 - 60);
    } else if (distance >= 70) {
        this->DistanceMemberFunc["very far"] = 1.0f;
    }


    if (speed <= 5) {
        this->SpeedMemberFunc["slow"] = 1.0f;
    } else if (speed > 5 && speed < 15) {
        this->SpeedMemberFunc["slow"] = 1 + (speed - 5) * (0 - 1) / (15 - 5);
    } else if (distance >= 15) {
        this->SpeedMemberFunc["slow"] = 0.0f;
    }

    if (speed <= 5) {
        this->SpeedMemberFunc["moderate"] = 0.0f;
    } else if (speed > 5 && speed < 15) {
        this->SpeedMemberFunc["moderate"] = 0 + (speed - 5) * (1 - 0) / (15 - 5);
    } else if (distance == 15) {
        this->SpeedMemberFunc["moderate"] = 1.0f;
    } else if (speed < 15 && speed < 25) {
        this->SpeedMemberFunc["moderate"] = 1 + (speed - 15) * (0 - 1) / (25 - 15);
    } else if (distance >= 25) {
        this->SpeedMemberFunc["moderate"] = 0.0f;
    }

    if (speed <= 15) {
        this->SpeedMemberFunc["fast"] = 0.0f;
    } else if (speed > 15 && speed < 25) {
        this->SpeedMemberFunc["fast"] = 0 + (speed - 15) * (1 - 0) / (25 - 15);
    } else if (distance >= 25) {
        this->SpeedMemberFunc["fast"] = 1.0f;
    }

    this->Action["hold"] = std::max(this->DistanceMemberFunc["far"], this->SpeedMemberFunc["slow"]);
    this->Action["walk"] = std::max(this->DistanceMemberFunc["close"], this->SpeedMemberFunc["fast"]);
    this->Action["run"] = std::min(this->DistanceMemberFunc["close"], this->SpeedMemberFunc["fast"]);

    glm::vec3 diff = this->Position - enemy.Position;
    if (Action["hold"] > Action["walk"] && Action["hold"] > Action["run"]) {
        this->Velocity = glm::normalize(diff) * 2.0f;
    } else if (Action["walk"] > Action["hold"] && Action["walk"] > Action["run"]) {
        this->Velocity = glm::normalize(diff) * 5.0f;
    } else if (Action["run"] > Action["hold"] && Action["run"] > Action["walk"]) {
        this->Velocity = glm::normalize(diff) * 10.0f;
    }
}

void AgentObject::Edge(GLint x_min, GLint x_max, GLint y_min, GLint y_max, GLint z_min, GLint z_max) {
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
        this->Velocity.y = -this->Velocity.y * 0.0f;
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

GLboolean AgentObject::GrowUp() {
    // std::cout << this->FoodAmount << std::endl;
    if (this->FoodAmount < 6) {
        this->FoodAmount += 1;
        this->Radius += 0.05;
        return GL_TRUE;
    } else {
        return GL_FALSE;
    }
}

void AgentObject::Hungry() {
    if (this->FoodAmount < 1) {
        this->Die();
    } else {
        this->FoodAmount -= 1;
        this->Radius -= 0.05;
    }
}

void AgentObject::Die() {
    this->Destroyed = GL_TRUE;
}