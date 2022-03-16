#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "GameObject.h"
#include "BallObject.h"
#include <map>
#include <string>

enum AgentState {
    STATE_SEARCH,
    STATE_FOOD,
    STATE_SEX,
    STATE_FIGHT,
    STATE_RUN
};

class AgentObject : public GameObject {
public:
    GLfloat Radius;
    GLfloat VisualRadius;
    GLfloat Elasticities;
    GLfloat LastFoodFrame;
    GLint FoodAmount;
    glm::vec3 FoodTarget = glm::vec3(0.0f);
    AgentState CurrentState = STATE_SEARCH;
    std::map<std::string, GLfloat> Action = {
            std::make_pair("hold", 0.0f),
            std::make_pair("walk", 0.0f),
            std::make_pair("run", 0.0f),
    };
    std::map<std::string, GLfloat> DistanceMemberFunc = {
            std::make_pair("very close", 0.0f),
            std::make_pair("close", 0.0f),
            std::make_pair("far", 0.0f),
            std::make_pair("very far", 0.0f)
    };

    std::map<std::string, GLfloat> SpeedMemberFunc = {
            std::make_pair("slow", 0.0f),
            std::make_pair("moderate", 0.0f),
            std::make_pair("fast", 0.0f)
    };

    AgentObject();
    AgentObject(glm::vec3 position, glm::vec3 velocity = glm::vec3(0.0f), GLfloat radius = 0.5f);

    void Update(GLfloat dt);

    void SearchFood(BallObject food);
    void SearchSex(AgentObject partner);
    void SearchEnemy(AgentObject enemy);

    void Runaway(AgentObject enemy);
    void Edge(GLint x_min, GLint x_max, GLint y_min, GLint y_max, GLint z_min, GLint z_max);
    GLboolean GrowUp();
    void Hungry();
    void Die();
};