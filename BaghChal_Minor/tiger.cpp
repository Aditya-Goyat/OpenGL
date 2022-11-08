#include "tiger.h"

Tiger::Tiger() {}

Tiger::Tiger(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity) : GameObject(pos, size, sprite, color, velocity) {}

void Tiger::Trapped() {
	isTrapped = true;
}

void Tiger::unTrapped() {
	isTrapped = false;
}