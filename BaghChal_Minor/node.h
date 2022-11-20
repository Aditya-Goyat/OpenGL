#pragma once
#include <glm/glm.hpp>
#include <vector>

class Node {
private:
	glm::vec2 position;

public:
	std::vector<Node> connected;
	bool isActive = false;
	bool isOccupied = false;
	
	glm::vec2 GetPosition() {
		return position;
	}

	void SetPosition(glm::vec2 position)
	{
		this->position = position;
	}
};