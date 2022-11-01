#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "text_renderer.h"
#include <sstream>
#include <iostream>


SpriteRenderer* Renderer;
TextRenderer* Text;

Game::Game(unsigned int width, unsigned int height)
    : Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
    delete Renderer;
    delete Text;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader myShader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(myShader);
    // load textures
    ResourceManager::LoadTexture("textures/arrowleft.png", true, "arrow");
    ResourceManager::LoadTexture("textures/box.png", true, "box");
    ResourceManager::LoadTexture("textures/hole.png", true, "hole");

    //Text
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("fonts/arial.ttf", 24);
}

void Game::Update(float dt)
{ 

}

void Game::ProcessInput(float dt)
{
   
}

void Game::Render()
{
    Texture2D boxTexture = ResourceManager::GetTexture("box");
    Renderer->DrawSprite(boxTexture, glm::vec2(100.0f, 250.0f), glm::vec2(400.0f, 200.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    Texture2D holeTexture = ResourceManager::GetTexture("hole");
    Renderer->DrawSprite(holeTexture, glm::vec2(230.0f, 260.0f), glm::vec2(80.0f, 70.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    Texture2D arrowTexture = ResourceManager::GetTexture("arrow");
    Renderer->DrawSprite(arrowTexture, glm::vec2(460.0f, 320.0f), glm::vec2(50.0f, 50.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    Renderer->DrawSprite(arrowTexture, glm::vec2(190.0f, 235.0f), glm::vec2(70.0f, 70.0f), 250.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    Text->RenderText("Hole dia = 20mm", 60, 225, 1);
    Text->RenderText("Object", 510, 330, 1);
    Text->RenderText("Drill Bit", 350, 50, 1.3);
}