#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "text_renderer.h"
#include "tiger.h"
#include "sheep.h"
#include "board.h"
#include <sstream>
#include <iostream>
#include "scenemanager.h"


SpriteRenderer* Renderer;
GameObject* Player;
TextRenderer* Text;
Tiger* tiger[3];
Sheep* sheep[15];
GameObject* background, *testNode[23];
Board* board;
Node* nodeUnderSheep = nullptr, *nodeUnderTiger = nullptr;
int fps;

//Button Game Object
GameObject* button_Play, *button_Options, *button_Exit;

void InitSheep(glm::vec2 startPos);
void InitTiger();
bool CheckButtonClick(glm::vec2 clickPos, GameObject* button);
Tiger* GetActiveTiger();
Sheep* GetSheepOverNode(Node* node);
float movementTime = 0.5f;

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_MENU), Keys(), Width(width), Height(height), turn(SHEEP)
{

}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Text;
}

void Game::Init()
{
	// load shaders
	ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");

	//load textures
	ResourceManager::LoadTexture("Textures/MainMenuBackground.png", true, "mm_background");
	ResourceManager::LoadTexture("Textures/Node.png", true, "Node");
	ResourceManager::LoadTexture("Textures/Sheep.png", true, "sheep");
	ResourceManager::LoadTexture("Textures/PlayBackground.png", true, "PlayBg");
	ResourceManager::LoadTexture("Textures/button_Play.png", true, "button_Play");
	ResourceManager::LoadTexture("Textures/button_Options.png", true, "button_Options");
	ResourceManager::LoadTexture("Textures/button_Exit.png", true, "button_Exit");
	ResourceManager::LoadTexture("Textures/Tiger.png", true, "tiger");

	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
		static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// set render-specific controls
	Shader myShader = ResourceManager::GetShader("sprite");
	Renderer = new SpriteRenderer(myShader);

	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("fonts/ocraext.TTF", 24);

	background = new GameObject(glm::vec2(0, 0), glm::vec2(Width, Height), ResourceManager::GetTexture("mm_background"));

	board = new Board();

	InitTiger();

	InitSheep(glm::vec2(87, 252));

	//Button Game Objets
	button_Play = new GameObject(glm::vec2(735, 330), glm::vec2(405, 118), ResourceManager::GetTexture("button_Play"));
	button_Options = new GameObject(glm::vec2(735, 498), glm::vec2(405, 118), ResourceManager::GetTexture("button_Options"));
	button_Exit = new GameObject(glm::vec2(735, 834), glm::vec2(405, 118), ResourceManager::GetTexture("button_Exit"));
	SceneManager::LoadScene(0, &State);


}

void Game::Update(float dt, GLFWwindow* window)
{
	fps = 60.0 / dt;

	int numberOfSheepEaten = 0;
	for (Sheep*& currSheep : sheep) {
		if (currSheep->isEaten)
			numberOfSheepEaten++;
		currSheep->Update(dt);
	}

	if (numberOfSheepEaten >= 5)
		glfwSetWindowShouldClose(window, true);

	for (Tiger* currTiger : tiger) {
		currTiger->Update(dt);
	}

	SetStuckSheep();
}

void Game::ProcessInput(float dt, GLFWwindow* window)
{
	if (Keys[GLFW_KEY_SPACE] && !keysProcessed[GLFW_KEY_SPACE]) {
		keysProcessed[GLFW_KEY_SPACE] = true;
		SceneManager::LoadScene(1, &State);
	}

	if (Mouse[0] && !mouseProcessed[0]) {
		mouseProcessed[0] = true;
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glm::vec2 mouseClick = glm::vec2(xpos, ypos);
		Sheep* activeSheep;
		Tiger* activeTiger;
		Node* nodeClicked;

		if (State == GAME_MENU) {
			if (ChangeScene(mouseClick, window))
				return;
		}

		Sheep* sheepClicked = CheckSheepClick(mouseClick);
		if (sheepClicked != nullptr) {
			if (turn == TIGER)
				return;
			if (sheepClicked->GetStuck())
				return;
			if (sheepClicked->sheepState == SHEEP_PLACED) {

				if (!CheckIfAllSheepPlaced())
					return;

				nodeUnderSheep = GetNodeUnderGameObject(sheepClicked);
			}
			if (sheepClicked->Velocity == glm::vec2(0, 0)) {
				for (Tiger* currTiger : tiger) {
					currTiger->IsActive = false;
					if (currTiger->GetStuck())
						continue;
					currTiger->Color = glm::vec3(1, 1, 1);
				}
				for (Sheep* currSheep : sheep) {
					currSheep->IsActive = false;
					if (currSheep->GetStuck())
						continue;
					currSheep->Color = glm::vec3(1, 1, 1);
				}
				sheepClicked->IsActive = true;
				sheepClicked->Color = glm::vec3(0, 1, 0);
			}

			return;
		}

		Tiger* tigerClicked = CheckTigerClick(mouseClick);
		if (tigerClicked != nullptr) {
			if (turn == SHEEP)
				return;
			if (tigerClicked->GetStuck())
				return;
			
			nodeUnderTiger = GetNodeUnderGameObject(tigerClicked);

			if (tigerClicked->Velocity == glm::vec2(0, 0)) {
				for (Tiger* currTiger : tiger) {
					currTiger->IsActive = false;
					if (currTiger->GetStuck())
						continue;
					currTiger->Color = glm::vec3(1, 1, 1);
				}
				for (Sheep* currSheep : sheep) {
					currSheep->IsActive = false;
					if (currSheep->GetStuck())
						continue;
					currSheep->Color = glm::vec3(1, 1, 1);
				}
				tigerClicked->IsActive = true;
				tigerClicked->Color = glm::vec3(0, 1, 0);
			}

			return;
		}

		activeSheep = GetActiveSheep();
		activeTiger = GetActiveTiger();
		//if (activeSheep == nullptr)
		//	return;

		nodeClicked = CheckNodeClick(mouseClick);

		if (nodeClicked != nullptr) {

			if (activeSheep != nullptr) {
				if (nodeUnderSheep != nullptr) {
					bool flag = false;
					for (auto itr = nodeUnderSheep->connected.begin(); itr != nodeUnderSheep->connected.end(); itr++) {
						if ((*itr)->GetPosition() == nodeClicked->GetPosition()) {
							flag = true;
							nodeUnderSheep->SetOccupiedBySheep(false);
							break;
						}
					}

					if (!flag)
						return;
				}

				activeSheep->endPos = (nodeClicked->GetPosition() + glm::vec2(23, 23)) - (activeSheep->Size) * 0.5f;
				float dist = glm::length(activeSheep->endPos - activeSheep->Position);
				activeSheep->speed = dist / movementTime;
				activeSheep->IsActive = false;
				activeSheep->sheepState = SHEEP_PLACED;
				turn = TIGER;

			}

			if (activeTiger != nullptr) {
				if (nodeUnderTiger != nullptr) {
					bool flag = false;
					for (auto itr = nodeUnderTiger->connected.begin(); itr != nodeUnderTiger->connected.end(); itr++) {
						if ((*itr)->GetPosition() == nodeClicked->GetPosition()) {
							flag = true;
							nodeUnderTiger->SetOccupiedByTiger(false);
							break;
						}
					}

					if (!flag) {
						bool secondFlag = false;
						Node* nodeOccupiedBySheep = nullptr;
						for (auto itr = nodeUnderTiger->connected.begin(); itr != nodeUnderTiger->connected.end(); itr++) {
							if ((*itr)->GetOccupiedBySheep() == true) {
								nodeOccupiedBySheep = (*itr);
							}

							if (nodeOccupiedBySheep != nullptr) {
								for (auto itr = nodeOccupiedBySheep->connected.begin(); itr != nodeOccupiedBySheep->connected.end(); itr++) {
									if ((*itr)->GetPosition() == nodeClicked->GetPosition()) {
										secondFlag = true;
										nodeUnderTiger->SetOccupiedByTiger(false);
										Sheep* sheepOverNode = GetSheepOverNode(nodeOccupiedBySheep);
										sheepOverNode->endPos = glm::vec2(1800, 900);
										sheepOverNode->isEaten = true;
										nodeOccupiedBySheep->SetOccupiedBySheep(false);
										break;
									}
								}
							}
						}

						if (!secondFlag)
							return;

						glm::vec2 dirFromTigerToSheep = glm::normalize(nodeOccupiedBySheep->GetPosition() - nodeUnderTiger->GetPosition());
						std::cout << dirFromTigerToSheep.x << " " << dirFromTigerToSheep.y << std::endl;
						glm::vec2 dirFromSheepToNodeClicked = glm::normalize(nodeClicked->GetPosition() - nodeOccupiedBySheep->GetPosition());
						std::cout << dirFromSheepToNodeClicked.x << " " << dirFromSheepToNodeClicked.y << std::endl;

						glm::vec2 errorVec = dirFromSheepToNodeClicked - dirFromTigerToSheep;

						if (glm::length(errorVec) >= 1.0f)
							return;
					}
				}

				activeTiger->endPos = (nodeClicked->GetPosition() + glm::vec2(23, 23)) - (activeTiger->Size) * 0.5f;
				float dist = glm::length(activeTiger->endPos - activeTiger->Position);
				activeTiger->speed = dist / movementTime;
				activeTiger->IsActive = false;
				turn = SHEEP;
			}
		}
	}
}

bool Game::CheckIfAllSheepPlaced()
{
	for (Sheep* currSheep : sheep) {
		if (currSheep->sheepState == SHEEP_PLACING)
			return false;
	}
	return true;
}

bool Game::ChangeScene(const glm::vec2& mouseClick, GLFWwindow* window)
{
	if (CheckButtonClick(mouseClick, button_Play)) {
		SceneManager::LoadScene(1, &State);
		return true;
	}
	else if (CheckButtonClick(mouseClick, button_Exit)) {
		glfwSetWindowShouldClose(window, true);
		return true;
	}
	else if (CheckButtonClick(mouseClick, button_Options)) {
		SceneManager::LoadScene(3, &State);
		return true;
	}
}

void Game::Render()
{

	if (State == GAME_PLAY) {
		Texture2D PlayBg = ResourceManager::GetTexture("PlayBg");
		Renderer->DrawSprite(PlayBg, glm::vec2(0, 0), glm::vec2(Width, Height));

		for (int i = 0; i < 15; i++) {
			sheep[i]->Draw(*Renderer);
		}

		for (int i = 0; i < 3; i++) {
			tiger[i]->Draw(*Renderer);
		}
	}

	if (State == GAME_MENU) {
		background->Draw(*Renderer);
		//button_Play->Draw(*Renderer);
		//button_Exit->Draw(*Renderer);
		//button_Options->Draw(*Renderer);
	}

	Text->RenderText(std::to_string(fps), 10, 10, 1);
}


void Game::ResetLevel()
{

}

void Game::ResetPlayer()
{

}

// collision detection
bool CheckCollision(GameObject& one, GameObject& two);

void Game::DoCollisions()
{

}

bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
	// collision x-axis?
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;
	// collision y-axis?
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;
	// collision only if on both axes
	return collisionX && collisionY;
}

void InitSheep(glm::vec2 startPos) {
	int counter = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 5; j++) {
			glm::vec2 position = glm::vec2(startPos.x + (i * 107), startPos.y + (j * 114));
			glm::vec2 size = glm::vec2(89, 106);

			sheep[counter] = new Sheep(position, size, ResourceManager::GetTexture("sheep"));
			sheep[counter]->IsActive = false;
			counter++;
		}
	}
}

void InitTiger() {
	glm::vec2 size(108, 85);
	tiger[0] = new Tiger((board->nodes[0].GetPosition() + glm::vec2(23, 23) - (size) * 0.5f), size, ResourceManager::GetTexture("tiger"));
	tiger[1] = new Tiger((board->nodes[3].GetPosition() + glm::vec2(23, 23) - (size) * 0.5f), size, ResourceManager::GetTexture("tiger"));
	tiger[2] = new Tiger((board->nodes[4].GetPosition() + glm::vec2(23, 23) - (size) * 0.5f), size, ResourceManager::GetTexture("tiger"));

}

Node* Game::CheckNodeClick(glm::vec2 clickPos)
{
	for (int i = 0; i < 23; i++)
	{
		if (clickPos.x >= board->nodes[i].GetPosition().x && clickPos.x <= board->nodes[i].GetPosition().x + 46)
			if (clickPos.y >= board->nodes[i].GetPosition().y && clickPos.y <= board->nodes[i].GetPosition().y + 46)
				return &board->nodes[i];
	}

	return nullptr;
}

Sheep* Game::CheckSheepClick(glm::vec2 clickPos)
{
	for (int i = 0; i < 15; i++)
	{
		if (clickPos.x >= sheep[i]->Position.x && clickPos.x <= sheep[i]->Position.x + sheep[i]->Size.x)
			if (clickPos.y >= sheep[i]->Position.y && clickPos.y <= sheep[i]->Position.y + sheep[i]->Size.y)
				return sheep[i];
	}

	return nullptr;
}

Tiger*  Game::CheckTigerClick(glm::vec2 clickPos)
{
	for (int i = 0; i < 3; i++)
	{
		if (clickPos.x >= tiger[i]->Position.x && clickPos.x <= tiger[i]->Position.x + tiger[i]->Size.x)
			if (clickPos.y >= tiger[i]->Position.y && clickPos.y <= tiger[i]->Position.y + tiger[i]->Size.y)
				return tiger[i];
	}

	return nullptr;
}

bool CheckButtonClick(glm::vec2 clickPos, GameObject* button)
{
	if (clickPos.x >= button->Position.x && clickPos.x <= button->Position.x + button->Size.x)
		if (clickPos.y >= button->Position.y && clickPos.y <= button->Position.y + button->Size.y)
			return true;

	return false;
}

Node* Game::GetActiveNode() {
	for (int i = 0; i < 23; i++) {
		if (board->nodes[i].isActive)
			return &board->nodes[i];
	}

	return nullptr;
}

Sheep* Game::GetActiveSheep() {
	for (Sheep*& currSheep : sheep) {
		if (currSheep->IsActive)
			return currSheep;
	}

	return nullptr;
}

Node* Game::GetNodeUnderGameObject(GameObject* gb) {
	for (int i = 0; i < 23; i++) {
		glm::vec2 pos = (board->nodes[i].GetPosition() + glm::vec2(23, 23) - (gb->Size) * 0.5f);
		if (pos == gb->Position)
			return &(board->nodes[i]);
	}

	return nullptr;
}

Sheep* GetSheepOverNode(Node* node) {
	for (Sheep* currSheep : sheep) {
		glm::vec2 pos = (node->GetPosition() + glm::vec2(23, 23) - (currSheep->Size * 0.5f));

		if (pos == currSheep->Position)
			return currSheep;
	}

	return nullptr;
}

void Game::SetOccupiedNodes() {
	for (Sheep* currSheep : sheep) {
		Node* nodeUnderSheep = GetNodeUnderGameObject(currSheep);
		if (nodeUnderSheep != nullptr) {
			nodeUnderSheep->SetOccupiedBySheep(true);
		}
	}

	for (Tiger* currTiger : tiger) {
		Node* nodeUnderTiger = GetNodeUnderGameObject(currTiger);
		if (nodeUnderTiger != nullptr) {
			nodeUnderTiger->SetOccupiedByTiger(true);
		}
	}
}

void Game::SetStuckSheep() {
	SetOccupiedNodes();

	for (Sheep* currSheep : sheep) {
		Node* nodeUnderSheep = GetNodeUnderGameObject(currSheep);
		if (nodeUnderSheep != nullptr) {
			bool flag = true;
			for (auto itr = nodeUnderSheep->connected.begin(); itr != nodeUnderSheep->connected.end(); itr++) {
				if ((*itr)->GetOccupiedBySheep() == false && (*itr)->GetOccupiedByTiger() == false) {
					flag = false;
					break;
				}
			}

			if (flag) {
				currSheep->Color = glm::vec3(1, 0, 0);
				currSheep->SetStuck(true);
			}
			else {
				if(!currSheep->IsActive)
					currSheep->Color = glm::vec3(1, 1, 1);
				currSheep->SetStuck(false);
			}
		}
	}

	bool allTigerStuck = true;
	for (Tiger* currTiger : tiger) {
		Node* nodeUnderTiger = GetNodeUnderGameObject(currTiger);
		if (nodeUnderTiger != nullptr) {
			bool flag = true;
			for (auto itr = nodeUnderTiger->connected.begin(); itr != nodeUnderTiger->connected.end(); itr++) {

				if ((*itr)->GetOccupiedByTiger())
					continue;

				if ((*itr)->GetOccupiedBySheep() == false && (*itr)->GetOccupiedByTiger() == false) {
					flag = false;
					break;
				}

				Node* connectedNode = (*itr);
				for (auto itr1 = connectedNode->connected.begin(); itr1 != connectedNode->connected.end(); itr1++) {

					glm::vec2 dirNodeUnderTigerToConnectedNode = glm::normalize(connectedNode->GetPosition() - nodeUnderTiger->GetPosition());
					glm::vec2 dirConnectedNodeToitr1 = glm::normalize((*itr1)->GetPosition() - connectedNode->GetPosition());

					glm::vec2 errorVec = dirNodeUnderTigerToConnectedNode - dirConnectedNodeToitr1;

					if (glm::length(errorVec) >= 1.0f)
						continue;

					if ((*itr1)->GetOccupiedBySheep() == false && (*itr1)->GetOccupiedByTiger() == false) {
						flag = false;
						break;
					}
				}

				if (!flag)
					break;
			}

			if (flag) {
				currTiger->Color = glm::vec3(1, 0, 0);
				currTiger->SetStuck(true);
			}
			else {
				if (!currTiger->IsActive)
					currTiger->Color = glm::vec3(1, 1, 1);

				currTiger->SetStuck(false);
			}
		}

		if (!currTiger->GetStuck())
			allTigerStuck = false;
	}

	if (allTigerStuck)
		SceneManager::LoadScene(0, &State);
}

Tiger* GetActiveTiger() {
	for (Tiger* currTiger : tiger) {
		if (currTiger->IsActive)
			return currTiger;
	}

	return nullptr;
}