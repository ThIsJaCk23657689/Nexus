#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "GameObject.h"

class BallObject : public GameObject {
public:
    GLfloat Radius;
    GLfloat Elasticities;

    BallObject();
    BallObject(glm::vec3 position, glm::vec3 velocity = glm::vec3(0.0f), GLfloat radius = 0.5f);

    void Update(GLfloat dt);
    void Edge(GLint x_min, GLint x_max, GLint y_min, GLint y_max, GLint z_min, GLint z_max);
};