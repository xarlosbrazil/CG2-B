#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class Mesh
{
public:

    Mesh(const std::vector<float>& vertices);
    ~Mesh();

    void draw();

    GLuint VAO, VBO;
    unsigned int vertexCount;

	glm::vec3 minBounds; // Minimum XYZ coordinates
	glm::vec3 maxBounds; // Maximum XYZ coordinates

};