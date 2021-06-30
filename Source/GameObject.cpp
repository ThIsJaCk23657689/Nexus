#include "GameObject.h"

GameObject::GameObject()
        : Position(0, 0, 0), Size(1.0f),
        Velocity(0.0f), Acceleration(0.0f),
        Force(0.0f), Mass(1.0f), IsSolid(GL_FALSE), Destroyed(GL_FALSE) {

    glm::mat4 model(1.0f);
    model = glm::translate(model, this->Position);
    model = glm::scale(model, this->Size);
    this->Model = model;

    this->Ambient = glm::vec4(0.02f, 0.02f, 0.02f, 1.0);
    this->Diffuse = glm::vec4(0.35f, 0.35f, 0.35f, 1.0);
    this->Specular = glm::vec4(0.45f, 0.55f, 0.45f, 1.0);
}

GameObject::GameObject(glm::vec3 position, glm::vec3 size,
                       glm::vec3 velocity, glm::vec3 acceleration,
                       glm::vec3 force, GLfloat mass)
        : Position(position), Size(size),
        Velocity(velocity), Acceleration(acceleration),
        Force(force), Mass(mass), IsSolid(GL_FALSE), Destroyed(GL_FALSE) {

    glm::mat4 model(1.0f);
    model = glm::translate(model, this->Position);
    model = glm::scale(model, this->Size);
    this->Model = model;

    this->Ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0);
    this->Diffuse = glm::vec4(0.35f, 0.35f, 0.35f, 1.0);
    this->Specular = glm::vec4(0.55f, 0.45f, 0.45f, 1.0);
}