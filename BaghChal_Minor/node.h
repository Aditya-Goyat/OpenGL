#pragma once
#include <glm/glm.hpp>
#include <vector>

class Node {
private:
	glm::vec2 position;
	bool isOccupiedBySheep;
	bool isOccupiedByTiger;

public:
	std::vector<Node*> connected;
	bool isActive;

	Node() : isOccupiedBySheep(false), isOccupiedByTiger(false), isActive(false) {

	}
	
	glm::vec2 GetPosition() {
		return position;
	}

	void SetPosition(glm::vec2 position)
	{
		this->position = position;
	}

	void SetOccupiedBySheep(bool isOccupied) {
		this->isOccupiedBySheep = isOccupied;
	}

	bool GetOccupiedBySheep() {
		return isOccupiedBySheep;
	}

	void SetOccupiedByTiger(bool isOccupied) {
		this->isOccupiedByTiger = isOccupied;
	}

	bool GetOccupiedByTiger() {
		return isOccupiedByTiger;
	}
};