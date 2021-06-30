#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tuple>

class GameObject {
public:
    glm::mat4 Model;
    glm::vec3 Position, Velocity, Acceleration, Force, Size;
    glm::vec4 Ambient, Diffuse, Specular;
    GLfloat Mass;
    GLboolean IsSolid;
    GLboolean Destroyed;

    GameObject();
    GameObject(glm::vec3 position, glm::vec3 size = glm::vec3(1.0f),
               glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 acceleration = glm::vec3(0.0f),
               glm::vec3 force = glm::vec3(0.0f), GLfloat mass = 1.0f);
};