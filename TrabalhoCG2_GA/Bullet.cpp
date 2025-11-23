#include "Bullet.h"

Bullet::Bullet(glm::vec3 pos, glm::vec3 dir)
{
	this->position = pos;
	this->direction = dir;

	this->speed = 10.0f;
	this->lifeTime = 3.0f;
	this->active = true;
	this->checkingCollision = true;
    
}

void Bullet::update(float deltaTime)
{
	if (!this->active) { // Se a bala não está ativa, não faz nada
        return;
    }

    // Atualiza o tempo de vida
	this->lifeTime -= deltaTime; // Diminui o tempo de vida baseado no deltaTime
    if (this->lifeTime <= 0.0f)
    {
        this->active = false;
        return; // Bala expirou
    }

    // Atualiza a posição
	this->position += this->direction * this->speed * deltaTime; // DeltaTime para movimento independente da taxa de quadros
}