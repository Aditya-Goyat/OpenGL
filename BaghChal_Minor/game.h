#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <vector>
#include "node.h"
#include "sheep.h"
#include "tiger.h"
#include "game_object.h"

enum GameState {
    GAME_PLAY,
    GAME_MENU,
    GAME_OVER,
    GAME_OPTIONS,
};

enum Turn {
    TIGER,
    SHEEP
};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;
    Turn                    turn;
    bool                    Keys[1024];
    bool                    Mouse[3];
    unsigned int            Width, Height;
    unsigned int            Lives;
    glm::vec2 endPos = glm::vec2(0, 0);
    bool keysProcessed[1024];
    bool mouseProcessed[3];
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt, GLFWwindow* window);
    bool CheckIfAllSheepPlaced();
    bool ChangeScene(const glm::vec2& mouseClick, GLFWwindow* window);
    void Update(float dt, GLFWwindow* window);
    void Render();
    void DoCollisions();
    //check click
    Sheep* CheckSheepClick(glm::vec2 clickPos);
    Tiger* CheckTigerClick(glm::vec2 clickPos);
    Node* CheckNodeClick(glm::vec2 clickPos);
    // reset
    void ResetLevel();
    void ResetPlayer();

    void SetStuckSheep();
    Node* GetNodeUnderGameObject(GameObject* gb);

private:
    Node* GetActiveNode();
    Sheep* GetActiveSheep();
    void SetOccupiedNodes();
};