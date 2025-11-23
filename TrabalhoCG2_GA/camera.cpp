#include "Camera.h"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up)
{
	// Inicializa os vetores e ângulos da câmera com os valores fornecidos.
    this->Position = position;
    this->WorldUp = up;

	this->Front = glm::vec3(0.0f, 0.0f, -1.0f); // A câmera inicialmente olha para o eixo -Z no sistema de coordenadas do OpenGL.

	this->MovementSpeed = 8.0f; // Define uma velocidade padrão de movimento.

    this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
    this->Up = glm::normalize(glm::cross(this->Right, this->Front));

}

// Implementação da Visão
glm::mat4 Camera::GetViewMatrix() // Retorna a matriz View (olhar através da câmera)
{
	// LookAt: método que cria uma matriz de visualização.
	return glm::lookAt(this->Position, this->Position + this->Front, this->Up); // glm::lookAt(eye, center, up)
    
}

void Camera::ProcessKeyboard(char direction, float deltaTime)

{
    float velocity = this->MovementSpeed * deltaTime;

    if (direction == 'W')
        this->Position += this->Front * velocity;
    if (direction == 'S')
        this->Position -= this->Front * velocity;

    if (direction == 'L' || direction == 'R') // Yaw
         
    {

        float rotationSpeed = 120.0f * deltaTime;
        float rotationAngle = rotationSpeed;

        if (direction == 'R') {
           
            rotationAngle = -rotationSpeed; // Invertido para R
        }

		// Rotaciona o vetor Front em torno do eixo WorldUp
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), this->WorldUp);

		// Aplica a rotação ao vetor Front
        this->Front = glm::vec3(rotation * glm::vec4(this->Front, 0.0f));

		// Recalcula os vetores Right e Up
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
        this->Up = glm::normalize(glm::cross(this->Right, this->Front));
    }
}
    