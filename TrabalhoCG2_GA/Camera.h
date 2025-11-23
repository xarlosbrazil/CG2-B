#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera

{
public:

	glm::vec3 Position;  // Posição da câmera no mundo.
	glm::vec3 Front; // A direção para onde a câmera está olhando. 
	glm::vec3 Up; // O vetor "para cima" da câmera.
	glm::vec3 Right; // O vetor "para a direita" da câmera.
	glm::vec3 WorldUp; // O vetor "para cima" do mundo (geralmente (0,1,0)).

    float MovementSpeed; // A velocidade de "caminhada" da câmera.

    // Construtor: A função que "cria" a câmera e a coloca em um estado inicial.
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 GetViewMatrix(); // Retorna a matriz View (olhar através da câmera)

    void ProcessKeyboard(char direction, float deltaTime);

};