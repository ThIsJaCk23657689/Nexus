#include "AgentObject.h"
#include <iostream>
#include <map>

AgentObject::AgentObject() : GameObject(), Radius(0.5f), Elasticities(1.0f) {
    this->Ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0);
    this->Diffuse = glm::vec4(0.25f, 0.25f, 1.0f, 1.0);
    this->Specular = glm::vec4(0.55f, 0.45f, 0.45f, 1.0);
    this->VisualRadius = this->Radius + 3.0f;
    this->FoodAmount = 3;
    this->LastFoodFrame = 0;
    this->CurrentState == STATE_SEARCH;
}

AgentObject::AgentObject(glm::vec3 position, glm::vec3 velocity, GLfloat radius)
        : GameObject(position, glm::vec3(radius),
                     velocity,glm::vec3(0.0f),
                     glm::vec3(0.0f), radius * 2), Radius(radius), Elasticities(1.0f) {
    this->Ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0);
    this->Diffuse = glm::vec4(0.25f, 0.25f, 1.0f, 1.0);
    this->Specular = glm::vec4(0.55f, 0.45f, 0.45f, 1.0);
    this->VisualRadius = this->Radius + 3.0f;
    this->FoodAmount = 3;
    this->LastFoodFrame = 0;
    this->CurrentState == STATE_SEARCH;
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
        // 如果該食物已經被吃掉了，請無視。
        return;
    }

    if (this->CurrentState == STATE_SEARCH) {
        // 代表是第一眼看到食物
        glm::vec3 diff = food.Position - this->Position;
        if (glm::length(diff) < this->VisualRadius) {
            // 如果有在可是範圍內 就優先過去第一眼看到的那個食物
            this->FoodTarget = food.Position;
            glm::vec3 force = (diff - this->Velocity) * 1.0f / glm::length(diff);;
            this->Force += force;
        }
        this->CurrentState = STATE_FOOD;
    } else if (this->CurrentState == STATE_FOOD) {
        // 代表之前已經看過其他食物了，只需比較現在這個食物有沒有比較近 有的話就過去
        glm::vec3 diff = food.Position - this->Position;
        if (glm::length(diff) < this->VisualRadius) {
            if (glm::distance(this->Position, this->FoodTarget) > glm::length(diff)) {
                this->FoodTarget = food.Position;
            }
            diff = this->FoodTarget - this->Position;
            glm::vec3 force = (diff - this->Velocity) * 1.0f / glm::length(diff);;
            this->Force += force;
        }
    }
}

void AgentObject::SearchSex(AgentObject partner) {
    if (partner.Destroyed) {
        // 如果該夥伴已經死了，請無視。
        return;
    }

    if (this->CurrentState == STATE_SEX) {
        // 如果目前的agent狀況是 STATE_SEX 才會想找伴侶
        glm::vec3 diff = partner.Position - this->Position;
        if (glm::length(diff) < this->VisualRadius) {
            glm::vec3 force = (diff - this->Velocity) * 1.0f / glm::length(diff);
            this->Force += force;
        }
    }
}

void AgentObject::SearchEnemy(AgentObject enemy) {
    if (enemy.Destroyed) {
        // 如果該夥伴已經死了，請無視。
        return;
    }

    if (this->CurrentState == STATE_SEARCH || this->CurrentState == STATE_FOOD || this->CurrentState == STATE_SEX) {
        // 如果目前的agent狀況是 STATE_SEARCH STATE_FOOD STATE_SEX 看到敵人都會進入警戒狀態
        // 如果自己比對方大，可以打架；反之逃跑。

        glm::vec3 diff = enemy.Position - this->Position;
        if (glm::length(diff) < this->VisualRadius) {
            if (this->Radius > enemy.Radius) {
                this->CurrentState = STATE_FIGHT;
            } else {
                this->CurrentState = STATE_RUN;
            }
            // 行為交給Fuzzy Logic去執行
            this->Runaway(enemy);
        } else {
            if (this->FoodAmount == 6) {
                this->CurrentState = STATE_SEX;
            } else {
                this->CurrentState = STATE_SEARCH;
            }
        }
    }
}

void AgentObject::Runaway(AgentObject enemy) {
    // 先取得對方的資訊
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

    // 算出敵人指向本身的向量
    glm::vec3 diff = glm::vec3(0.0f);
    if (this->CurrentState == STATE_FIGHT) {
        // 追殺敵人
        diff = this->Position - enemy.Position;
    } else if (this->CurrentState == STATE_RUN) {
        // 逃跑實在
        diff = this->Position - enemy.Position;
    }
    this->Force += (diff) * (this->Action["hold"] * 1.0f) * (this->Action["walk"] * 2.0f) * (this->Action["run"] * 5.0f);

    // this->Velocity = Action["hold"] * glm::normalize(diff) * 0.1f +  Action["walk"] * glm::normalize(diff) * 2.0f + Action["run"] * glm::normalize(diff) * 5.0f;
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
    if (this->FoodAmount < 6) {
        // 飽滿度沒有滿 就可以長大
        this->FoodAmount += 1;
        this->Radius += 0.05;

        // 吃完又會變成尋找狀態
        this->CurrentState = STATE_SEARCH;
        this->FoodTarget = glm::vec3(0.0f);

        return GL_TRUE;

    } else {
        // 飽滿度滿了 就不用吃食物 尋找伴侶重要
        this->CurrentState = STATE_SEX;
        this->FoodTarget = glm::vec3(0.0f);

        return GL_FALSE;
    }
}

void AgentObject::Hungry() {
    if (this->FoodAmount == 0) {
        this->Die();
    } else {
        this->FoodAmount -= 1;
        this->Radius -= 0.05;
        this->CurrentState = STATE_SEARCH;
    }
}

void AgentObject::Die() {
    this->Destroyed = GL_TRUE;
}