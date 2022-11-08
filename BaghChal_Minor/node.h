#pragma once
#include <glm/glm.hpp>
#include <vector>

class Node {
private:
	glm::vec2 position;

public:
	std::vector<Node> connected;
	
	glm::vec2 GetPosition() {
		return position;
	}
};