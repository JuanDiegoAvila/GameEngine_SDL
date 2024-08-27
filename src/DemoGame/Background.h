#pragma once

#include <string>
#include "Engine/Entity.h"
#include "Engine/Systems.h"
#include "Engine/Components.h"
#include "Components.h"
#include "Tilemap.h"
#include "Sprites.h"

class BackgroundSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* background = scene->createEntity("Background");
    const std::string& bgfile = "assets/Background/Background-Sheet.png";
    background->addComponent<PositionComponent>(0, 0);
    background->addComponent<TextureComponent>(bgfile);
    background->addComponent<SpriteComponent>(bgfile, 1024, 768, 1, 4, 1000);
    background->addComponent<BackgroundComponent>(bgfile);
  }
};
