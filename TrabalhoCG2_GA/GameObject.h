#pragma once

#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>

struct AABB
{

    glm::vec3 min;
    glm::vec3 max;

};

class GameObject
{
public:

    Mesh* mesh;
    glm::mat4 transform;
    bool isEliminable;
    bool active;

    AABB getAABB();

    GameObject(Mesh* mesh);

    void draw(Shader& shader);
};