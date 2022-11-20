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
GameObject* background, * testNode[23];
Board* board;

//Button Game Object
GameObject* button;

void InitSheep(glm::vec2 startPos);
float movementTime = 0.5f;

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_MENU), Keys(), Width(width), Height(height)
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

	//Node Test
	testNode[0] = new GameObject(glm::vec2(1033, 47), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[1] = new GameObject(glm::vec2(510, 322), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[2] = new GameObject(glm::vec2(892, 320), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[3] = new GameObject(glm::vec2(984, 320), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[4] = new GameObject(glm::vec2(1082, 320), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[5] = new GameObject(glm::vec2(1171, 320), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[6] = new GameObject(glm::vec2(1556, 320), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[7] = new GameObject(glm::vec2(510, 475), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[8] = new GameObject(glm::vec2(817, 476), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[9] = new GameObject(glm::vec2(958, 476), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[10] = new GameObject(glm::vec2(1111, 476), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[11] = new GameObject(glm::vec2(1248, 476), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[12] = new GameObject(glm::vec2(1555, 476), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[13] = new GameObject(glm::vec2(510, 661), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[14] = new GameObject(glm::vec2(722, 661), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[15] = new GameObject(glm::vec2(925, 661), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[16] = new GameObject(glm::vec2(1144, 661), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[17] = new GameObject(glm::vec2(1340, 661), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[18] = new GameObject(glm::vec2(1555, 661), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[19] = new GameObject(glm::vec2(572, 962), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[20] = new GameObject(glm::vec2(873, 962), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[21] = new GameObject(glm::vec2(1197, 962), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));
	testNode[22] = new GameObject(glm::vec2(1492, 962), glm::vec2(46, 46), ResourceManager::GetTexture("Node"));

	board = new Board();

	//Create tiger and sheep game objects.
	for (int i = 0; i < 3; i++)
		tiger[i] = new Tiger();

	InitSheep(glm::vec2(87, 252));
	SceneManager::LoadScene(0, &State);
}

void Game::Update(float dt)
{
	for (Sheep*& currSheep : sheep) {
		currSheep->Update(dt);
	}
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
		Node* nodeClicked;

		Sheep* sheepClicked = CheckSheepClick(mouseClick);
		if (sheepClicked != nullptr) {
			if (sheepClicked->sheepState == SHEEP_PLACED) {
				for (Sheep* currSheep : sheep) {
					if (currSheep->sheepState == SHEEP_PLACING)
						return;
				}
			}
			if (sheepClicked->Velocity == glm::vec2(0, 0)) {
				for (Sheep* currSheep : sheep) {
					currSheep->Color = glm::vec3(1, 1, 1);
					currSheep->IsActive = false;
				}
				sheepClicked->IsActive = true;
				sheepClicked->Color = glm::vec3(0, 1, 0);
			}

			return;
		}

		activeSheep = GetActiveSheep();
		if (activeSheep == nullptr)
			return;

		nodeClicked = CheckNodeClick(mouseClick);
		if (nodeClicked != nullptr) {
			nodeClicked->isActive = true;

			activeSheep->endPos = (nodeClicked->GetPosition() + glm::vec2(23, 23)) - (activeSheep->Size) * 0.5f;
			float dist = glm::length(activeSheep->endPos - activeSheep->Position);
			activeSheep->speed = dist / movementTime;
			nodeClicked->isActive = false;
			activeSheep->IsActive = false;
			activeSheep->sheepState = SHEEP_PLACED;
		}
	}
}

void Game::Render()
{
	if (State == GAME_PLAY) {
		Texture2D PlayBg = ResourceManager::GetTexture("PlayBg");
		Renderer->DrawSprite(PlayBg, glm::vec2(0, 0), glm::vec2(Width, Height));

		for (int i = 0; i < 23; i++) {
			testNode[i]->Draw(*Renderer);
		}

		for (int i = 0; i < 15; i++) {
			sheep[i]->Draw(*Renderer);
		}
		return;
	}

	background->Draw(*Renderer);
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