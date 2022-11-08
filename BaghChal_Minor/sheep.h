#pragma once
#include "game_object.h"

class Sheep : public GameObject {

private:
	bool isEaten = false;

public:
	Sheep();
	Sheep(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	void Eat();
};