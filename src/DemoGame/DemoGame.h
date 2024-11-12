// #include "Engine/Game.h"
// #include "Engine/Scene.h"
// #include "Engine/Entity.h"
// #include "Engine/Components.h"
// #include "Engine/Systems.h"
// #include "Engine/Graphics/TextureManager.h"
// #include <printf.h>
// #include <entt/entt.hpp>
// #include "Player.h"
// #include "Tilemap.h"
// #include "Components.h"
// #include "Sprites.h"

// #include "Colliders.h"
// #include "Rock.h"

#pragma once

#include "DemoGame/Tilemap.h"
#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include <entt/entt.hpp>
#include "Sprites.h"
#include "Player.h"
#include "Background.h"
#include "Rock.h"
#include "Colliders.h"
#include "Sound.h"

class PlayerSpawnSetupSystem : public SetupSystem {
  void run() {
    Entity* square = scene->createEntity("NAVE", WIDTH/2 - 40, HEIGHT/2 - 40);
    square->addComponent<PlayerComponent>();
    square->addComponent<VelocityComponent>(300);
    square->addComponent<TextureComponent>("assets/Sprites/Nave.png");
    square->addComponent<SpriteComponent>("assets/Sprites/Nave.png", 16, 16, 5, 3, 400);
    square->get<SpriteComponent>().layer = 1;
    square->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 80, 80}, SDL_Color{255, 0, 0, 255});
  }
};

class RocksSpawnerSetupSystem : public SetupSystem {
  void run() {
    auto startPositionX = rand() % WIDTH;
    Entity* rockSpawner = scene->createEntity("ROCK_SPAWNER", startPositionX, 0);
    rockSpawner->addComponent<RockSpawner>(0, 1000, "assets/Sprites/Rock.png");
  }
};

class RockTextureSetupSystem : public SetupSystem {
  void run() {
    TextureManager::LoadTexture("assets/Sprites/Rock.png", scene->renderer);
  }
};

class RockRandomSpawnSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<RockSpawner>();
    Uint32 now = SDL_GetTicks();
    
    for (auto e : view) {
      auto& spawner = view.get<RockSpawner>(e);

      if (spawner.lastSpawnTime == 0) {
        spawner.lastSpawnTime = now;
        continue;
      }
      
      float timeSinceLastSpawn = now - spawner.lastSpawnTime;

      if (timeSinceLastSpawn > spawner.spawnRate) {
        printf("Rock Spawned\n");
        Entity* newRock = scene->createEntity("ROCK", rand() % WIDTH, 0);
        newRock->addComponent<VelocityComponent>(100, 0, 100);
        newRock->addComponent<RockComponent>();
        newRock->addComponent<TextureComponent>(spawner.filename);
        newRock->addComponent<SpriteComponent>(spawner.filename, 16, 16, 5, 10, 1000);
        newRock->get<SpriteComponent>().layer = 1;
        newRock->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 80, 80}, SDL_Color{255, 0, 0, 255});
        spawner.lastSpawnTime = now;
      }
    }
  }
};

class MovementSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<PositionComponent, VelocityComponent>();

    for (auto e : view) {
      auto& pos = view.get<PositionComponent>(e);
      auto vel = view.get<VelocityComponent>(e);

      pos.x += vel.x * dT;
      pos.y += vel.y * dT;
    }
  }
};

class WallHitSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<PositionComponent, VelocityComponent, SpriteComponent>();

    for (auto e : view) {
      auto pos = view.get<PositionComponent>(e);
      auto spr = view.get<SpriteComponent>(e);
      auto& vel = view.get<VelocityComponent>(e);
      
      int newPosX = pos.x + vel.x * dT;
      int newPosY = pos.y + vel.y * dT;

      if (newPosX < 0 || newPosX + spr.width > 1024) {
        vel.x *= -1.1;
        pos.x = 0 + (newPosX < 0 ? 0 + spr.width : 1024 - spr.width);

      }

      if(newPosY < 0) {
        vel.y *= -1.1;
        pos.y = 0;
      }
      
    }
  }
};

class MovementInputSystem : public EventSystem {
  void run(SDL_Event event) {
    auto view = scene->r.view<VelocityComponent, PlayerComponent>();

    for (auto e : view) {
      auto& vel = view.get<VelocityComponent>(e);

      if (event.type == SDL_KEYDOWN){
        switch (event.key.keysym.sym) {
          case SDLK_LEFT:
            vel.x = -vel.speed;
            break;
          case SDLK_RIGHT:
            vel.x = vel.speed;
            break;
          case SDLK_UP:
            vel.y = -vel.speed;
            break;
          case SDLK_DOWN:
            vel.y = vel.speed;
            break;
        }
        
      }else if (event.type == SDL_KEYUP){
        switch (event.key.keysym.sym) {
          case SDLK_LEFT:
          case SDLK_RIGHT:
            vel.x = 0;
            break;
          case SDLK_UP:
          case SDLK_DOWN:
            vel.y = 0;
            break;
        }
      }

    }
  }
};

class TextureSetupSystem : public SetupSystem {
  void run(){
    auto view = scene->r.view<TextureComponent>();
    for (auto e : view) {
      auto tex = view.get<TextureComponent>(e);
      TextureManager::LoadTexture(tex.filename, scene->renderer);
    }
  }
};

class CameraSetupSystem : public SetupSystem {
  void run() {
    int width = WIDTH;
    int height = HEIGHT;

    scene->mainCamera = scene->createEntity("CAMERA", 0, 0);
    scene->mainCamera->addComponent<CameraComponent>(
      1,
      width,
      height,
      width * 10,
      height * 10
    );
  }
};

class DemoGame : public Game {
  public:
    Scene* sampleScene;
    entt::registry r;

    DemoGame()
    : Game("BREAKOUT", WIDTH, HEIGHT) { }

    void setup() override {
      
      sampleScene = new Scene("Galaga", r, renderer);

      addSetupSystem<PlayerSpawnSetupSystem>(sampleScene);
      addSetupSystem<RocksSpawnerSetupSystem>(sampleScene);
      addSetupSystem<RockTextureSetupSystem>(sampleScene);
      addSetupSystem<BackgroundSetupSystem>(sampleScene);
      addSetupSystem<LivesSetupSystem>(sampleScene);
      addSetupSystem<TextureSetupSystem>(sampleScene);
      addSetupSystem<TilemapEntitySetupSystem>(sampleScene);
      addEventSystem<MovementInputSystem>(sampleScene);

      addSetupSystem<BackgroundMusicSetupSystem>(sampleScene);
      addSetupSystem<FxMusicSetupSystem>(sampleScene);
      addSetupSystem<SoundSetupSystem>(sampleScene);
      addSetupSystem<BackgroundMusicPlaySetupSystem>(sampleScene);

      addUpdateSystem<ColliderResetSystem>(sampleScene);
      addUpdateSystem<RockRandomSpawnSystem>(sampleScene);
      addUpdateSystem<SpriteMovementSystem>(sampleScene);
      addUpdateSystem<RockMovementSystem>(sampleScene);

      addUpdateSystem<PlayerRockCollisionDetectionSystem>(sampleScene);
      addUpdateSystem<PlayerCollisionSystem>(sampleScene);

      addUpdateSystem<PlayerTileCollisionDetectionSystem>(sampleScene);
      addUpdateSystem<PlayerWallCollisionSystem>(sampleScene);

      addUpdateSystem<MovementSystem>(sampleScene);
      addUpdateSystem<SpriteAnimationSystem>(sampleScene);
      addUpdateSystem<WallHitSystem>(sampleScene);
      addRenderSystem<SpriteRenderSystem>(sampleScene);
      addRenderSystem<LivesRenderSystem>(sampleScene);
      addRenderSystem<ColliderRenderSystem>(sampleScene);

      setScene(sampleScene);
    }
}; 
