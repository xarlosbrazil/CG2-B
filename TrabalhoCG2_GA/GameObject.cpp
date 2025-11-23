#include "GameObject.h"
#include <glm/gtc/type_ptr.hpp>

GameObject::GameObject(Mesh* mesh) // O mesh do objeto é reutilizado para vários objetos
{
    this->mesh = mesh;
	this->transform = glm::mat4(1.0f); // Identidade para iniciar
    this->isEliminable = false;
	this->active = true;
}

void GameObject::draw(Shader& shader)
{
    GLuint modelLoc = glGetUniformLocation(shader.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->transform)); // Passa a matriz model para o shader

    if (this->mesh)
    {
        this->mesh->draw();
    }
}

AABB GameObject::getAABB() // Por que AABB? Porque é Axis-Aligned Bounding Box (Caixa Envolvente Alinhada aos Eixos)
{
    // Pega os limites locais da Mesh
	glm::vec3 localMin = this->mesh->minBounds;
	glm::vec3 localMax = this->mesh->maxBounds; 

	glm::vec4 worldMin = this->transform * glm::vec4(localMin, 1.0f); // Gera um mínimo global aplicando ao minimo local a transformação do objeto (translação, rotação, escala)
	glm::vec4 worldMax = this->transform * glm::vec4(localMax, 1.0f);

    AABB worldAABB;
    worldAABB.min = glm::vec3(worldMin); 
    worldAABB.max = glm::vec3(worldMax);

    return worldAABB;
}