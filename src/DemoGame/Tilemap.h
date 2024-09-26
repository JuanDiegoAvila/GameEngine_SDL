#pragma once

#include <string>
#include "Sprites.h"
#include "Engine/Entity.h"
#include "Player.h"
#include "Engine/Systems.h"
#include <iostream>

enum class TileType {
  NONE,
  WALL,
  TRIGGER,
};

struct Tile {
  int index;
  int tilemapIndex;
  TileType type;
};

struct TileComponent {
  Tile tile;
};

struct TilemapComponent {
  std::string filename;
  std::vector<Tile> tiles;
  int tileSize;
  int scale;
  int width;
  int height;
};

class LivesSetupSystem : public SetupSystem {
public:
  void run() override {

    std::string filename = "assets/Tilesets/Heart.png";
    int tileSize = 16;
    int tileScale = 4;
    
    Entity* livesEntity = scene->createEntity("LIVES", WIDTH/2 - 20 * tileScale * 2, 20);
    livesEntity->addComponent<LivesComponent>(4);
    livesEntity->addComponent<TextureComponent>(filename);

  }
};

class LivesRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    auto view = scene->r.view<LivesComponent, TextureComponent, PositionComponent>();

    for (auto e : view) {
      auto lives = view.get<LivesComponent>(e);
      auto tex = view.get<TextureComponent>(e);
      auto pos = view.get<PositionComponent>(e);

      Texture* texture = TextureManager::GetTexture(tex.filename);

      for (int i = 0; i < lives.lives; i++) {
        texture->render(
          scene->renderer,
          pos.x + i * 20 * 4,
          pos.y,
          16 * 4,
          16 * 4
        );
      }
    }
  }
}; 