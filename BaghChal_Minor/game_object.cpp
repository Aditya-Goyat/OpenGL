#include "game_object.h"
#include <iostream>

GameObject::GameObject()
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), prevVelocity(Velocity), Color(1.0f), Rotation(0.0f), Sprite(), IsActive(false), Destroyed(false), endPos(0.0f, 0.0f) {
}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Size(size), Velocity(velocity), prevVelocity(Velocity), Color(color), Rotation(0.0f), Sprite(sprite), IsActive(false), Destroyed(false), endPos(pos) { }

void GameObject::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

void GameObject::Move(float dt) {
    glm::vec2 diff = endPos - Position;
    float dist = glm::length(diff);
    Velocity = glm::normalize(diff);
    
    if (dist <= 0.1f) {
        Position = endPos;
        if (prevVelocity != glm::vec2(0, 0))
            Color = glm::vec3(1, 1, 1);
        Velocity = glm::vec2(0, 0);
    }

    prevVelocity = Velocity;
    Position += Velocity * dt * speed;
}

void GameObject::Update(float dt) {
    Move(dt);
}

//void GameObject::Move(glm::vec2 endPos, float dt) {
//    Position.y = Approach(Position.y, endPos.y, dt);
//    Position.x = Approach(Position.x, endPos.x, dt);
//}

float GameObject::Approach(float startPos, float endPos, float dt) {
    float diff = endPos - startPos;

    if (diff > dt)
        return startPos + (dt * speed);

    if (diff < -dt)
        return startPos - (dt * speed);

    return endPos;
}