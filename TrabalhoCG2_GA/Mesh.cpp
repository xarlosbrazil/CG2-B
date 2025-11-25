#include "Mesh.h"

Mesh::Mesh(const std::vector<float>& vertices) // -- stantard library - vector  -- tipo de variável : vetor dinâmico de floats
{

	this->vertexCount = vertices.size() / 8; // Cada vértice tem 8 floats (X, Y, Z, U, V, NX, NY, NZ)

    minBounds = glm::vec3(std::numeric_limits<float>::max());
    maxBounds = glm::vec3(std::numeric_limits<float>::lowest());

	for (size_t i = 0; i < vertices.size(); i += 8) // Percorre apenas os vértices (X, Y, Z), ignorando UVs
    {
		float x = vertices[i]; // Primeiro vértice do triângulo
		float y = vertices[i + 1]; // Segundo vértice do triângulo
		float z = vertices[i + 2]; // Terceiro vértice do triângulo

		// Atualiza os limites mínimo e máximo -- Terminará o loop com os maiores valores de X, Y e Z e os menores valores de X, Y e Z

		if (x < minBounds.x) minBounds.x = x; // Condicional inline: se x for menor que minBounds.x, então minBounds.x recebe o valor de x
        if (y < minBounds.y) minBounds.y = y;
        if (z < minBounds.z) minBounds.z = z;

        if (x > maxBounds.x) maxBounds.x = x;
        if (y > maxBounds.y) maxBounds.y = y;
        if (z > maxBounds.z) maxBounds.z = z;

    }

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO); // Ativa o VBO como buffer de vértices corrente
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW); // Copia os dados para a GPU

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Argumentos: índice do atributo, tamanho (3 para XYZ), tipo, normalizado?, stride (espaçamento entre atributos), offset (posição inicial do atributo no array)
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    // Limpa a memória da GPU
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
}

void Mesh::draw()
{
    // Simplesmente ativa o VAO e manda desenhar
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, this->vertexCount);
    glBindVertexArray(0);
}