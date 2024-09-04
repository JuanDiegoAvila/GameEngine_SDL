#pragma once
#include <string>
#include "Engine/Entity.h"
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Components.h"
#include "Sprites.h"

struct PlayerComponent { };

struct PowerUpComponent { };

class PlayerSpawnSetupSystem : public SetupSystem {
  void run() {
    Entity* square = scene->createEntity("NAVE", WIDTH/2, HEIGHT/2);
    square->addComponent<PlayerComponent>();
    square->addComponent<VelocityComponent>(300);
    square->addComponent<TextureComponent>("assets/Sprites/Nave.png");
    square->addComponent<SpriteComponent>("assets/Sprites/Nave.png", 16, 16, 5, 3, 400);
    //square->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 80, 80}, SDL_Color{255, 0, 0, 255});
  }
};