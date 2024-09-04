#pragma once
#include <string>
#include "Engine/Entity.h"
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Components.h"
#include "Sprites.h"
#include <printf.h>

struct RockComponent {};

class RocksSpawnSetupSystem : public SetupSystem {
  void run() {
    auto startPositionX = rand() % WIDTH;
    Entity* rock = scene->createEntity("ROCK", startPositionX, 0);
    rock->addComponent<VelocityComponent>(100, 0, 100);
    rock->addComponent<RockComponent>();
    rock->addComponent<TextureComponent>("assets/Sprites/Rock.png");
    rock->addComponent<SpriteComponent>("assets/Sprites/Rock.png", 16, 16, 5, 10, 1000);
  }
};

class RockMovementSystem : public UpdateSystem {
    void run(float dT) {
        auto view = scene->r.view<PositionComponent, VelocityComponent, RockComponent>();
        
        for (auto e : view) {
            auto& pos = view.get<PositionComponent>(e);
            auto vel = view.get<VelocityComponent>(e);

            pos.y += vel.y * dT;

            if (pos.y > HEIGHT) {
                scene->r.destroy(e);

                printf("Rock Destroyed\n");
            }
        }
    }
};