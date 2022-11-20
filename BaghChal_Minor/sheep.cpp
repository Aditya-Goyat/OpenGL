#include "sheep.h"

Sheep::Sheep() : sheepState(SHEEP_PLACING) {
	
}

Sheep::Sheep(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity) : sheepState(SHEEP_PLACING), GameObject(pos, size, sprite, color, velocity) {
	
}

void Sheep::Eat() {

}