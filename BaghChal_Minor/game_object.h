#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"


// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
    // object state
    glm::vec2   Position, Size, Velocity, endPos, prevVelocity;
    glm::vec3   Color;
    float       Rotation;
    bool        IsActive;
    bool        Destroyed;
    float speed = 500.0f;
    // render state
    Texture2D   Sprite;
    // constructor(s)
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    // draw sprite
    virtual void Draw(SpriteRenderer& renderer);
    // move sprite
    void Move(float dt);
    void Update(float dt);

    void SetStuck(bool isStuck);
    bool GetStuck();

private:
    float Approach(float startPos, float endPos, float dt);
    bool isStuck = true;
};