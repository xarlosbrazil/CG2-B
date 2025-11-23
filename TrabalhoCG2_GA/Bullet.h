#pragma once

#include <glm/glm.hpp>

class Bullet
{
public:

	glm::vec3 position;
	glm::vec3 direction;
	float speed;
	float lifeTime;
	bool active;
	bool checkingCollision;

	Bullet(glm::vec3 pos, glm::vec3 dir);

	void update(float deltaTime);

};