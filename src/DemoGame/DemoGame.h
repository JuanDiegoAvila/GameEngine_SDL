#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Engine/Graphics/TextureManager.h"
#include <printf.h>
#include <entt/entt.hpp>
#include "Tilemap.h"
#include "Components.h"
#include "Sprites.h"
#include "Background.h"

#define WIDTH 1024
#define HEIGHT 768
#define BRICK_WIDTH 90
#define BRICK_HEIGHT 30
#define BRICK_SPACING 10
#define PADDLE_WIDTH 150
#define PADDLE_HEIGHT 20

#define SPEED_LIMIT 600

class PlayerSpawnSetupSystem : public SetupSystem {
  void run() {
    Entity* square = scene->createEntity("NAVE", 0, 0);
    square->addComponent<PlayerComponent>();
    square->addComponent<VelocityComponent>(300);
    square->addComponent<TextureComponent>("assets/Sprites/Nave.png");
    square->addComponent<SpriteComponent>("assets/Sprites/Nave.png", 16, 16, 5, 3, 400);
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

class InputSystem : public EventSystem {
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

class DemoGame : public Game {
  public:
    Scene* sampleScene;
    entt::registry r;

  public:
    DemoGame()
      : Game("BREAKOUT", WIDTH, HEIGHT)
    { }

    void setup() {
      
      sampleScene = new Scene("Galaga", r, renderer);
      addSetupSystem<PlayerSpawnSetupSystem>(sampleScene);
      addSetupSystem<BackgroundSetupSystem>(sampleScene);
      
      addSetupSystem<TilemapSetupSystem>(sampleScene);
      addSetupSystem<AutoTilingSetupSystem>(sampleScene);
      addSetupSystem<TextureSetupSystem>(sampleScene);

      addEventSystem<InputSystem>(sampleScene);

      addUpdateSystem<SpriteMovementSystem>(sampleScene);
      addUpdateSystem<MovementSystem>(sampleScene);
      addUpdateSystem<SpriteAnimationSystem>(sampleScene);
      addUpdateSystem<WallHitSystem>(sampleScene);
    
      addRenderSystem<SpriteRenderSystem>(sampleScene);
      addRenderSystem<TilemapRenderSystem>(sampleScene);

      setScene(sampleScene);
    }
}; 
