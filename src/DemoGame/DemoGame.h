#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Engine/Graphics/TextureManager.h"
#include <printf.h>
#include <entt/entt.hpp>

#define WIDTH 1024
#define HEIGHT 768
#define BRICK_WIDTH 90
#define BRICK_HEIGHT 30
#define BRICK_SPACING 10
#define PADDLE_WIDTH 150
#define PADDLE_HEIGHT 20

#define SPEED_LIMIT 600

SDL_Color red = {0xFF, 0x00, 0x00, 0xFF};
SDL_Color orange = {0xFF, 0xA5, 0x00, 0xFF};
SDL_Color yellow = {0xFF, 0xFF, 0x00, 0xFF};
SDL_Color green = {0x00, 0xFF, 0x00, 0xFF};
SDL_Color blue = {0x00, 0x00, 0xFF, 0xFF};
SDL_Color purple = {0x80, 0x00, 0x80, 0xFF};
SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};


struct TextureComponent {
  std::string filename;
};

struct BackgroundComponent {
  std::string filename;
};

struct SpriteComponent {
  std::string filename;
  int width;
  int height;
  int scale = 1;
  int animationFrames = 0;
  int animationDuration = 0;
  Uint32 lastUpdate = 0;
  int xIndex = 0;
  int yIndex = 0;
};

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

class SpriteMovementSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<SpriteComponent, VelocityComponent>();

    for (auto e : view) {
      auto& spr = view.get<SpriteComponent>(e);
      auto vel = view.get<VelocityComponent>(e);

      spr.yIndex = 0;
    }
  }
};

// class CollisionSystem : public UpdateSystem {
// public:
//     void run(float dT) override {
//         auto view = scene->r.view<PositionComponent, VelocityComponent>();

//         for (auto entity : view) {
//             auto &pos = view.get<PositionComponent>(entity);
//             auto &vel = view.get<VelocityComponent>(entity);

//             if (scene->r.any_of<BallComponent>(entity)) {

//                 limitSpeed(vel);
                
//                 auto paddleView = scene->r.view<PositionComponent, PlayerControlledComponent>();
//                 for (auto paddleEntity : paddleView) {
//                     auto &paddlePos = paddleView.get<PositionComponent>(paddleEntity);
//                     if (checkCollision(pos, paddlePos, PADDLE_WIDTH, PADDLE_HEIGHT)) { // Paddle size (width=50, height=10)
//                         // Invertir la dirección de la pelota
//                         vel.y = -vel.y;
//                     }
//                 }

//                 // Colisión con los bricks
//                 auto brickView = scene->r.view<PositionComponent, BrickComponent>();
//                 for (auto brickEntity : brickView) {
//                     auto &brickPos = brickView.get<PositionComponent>(brickEntity);
//                     if (checkCollision(pos, brickPos, BRICK_WIDTH, BRICK_HEIGHT)) {
//                         // Colisión detectada, eliminar el bloque
//                         scene->destroyEntity(brickEntity);
//                         scene->decreaseBrickCount();

//                         // Invertir la dirección de la pelota
//                         vel.y = -vel.y;
//                         break;
//                     }
//                 }
//             }
//         }
//     }

//     void setScene(Scene *s) {
//         scene = s;
//     }

// private:
//     Scene *scene;

//     bool checkCollision(const PositionComponent &a, const PositionComponent &b, int width, int height) {
//         // Implementar lógica de detección de colisiones
//         return !(a.x + width < b.x ||
//                  a.x > b.x + width ||
//                  a.y + height < b.y ||
//                  a.y > b.y + height);
//     }

//     void limitSpeed(VelocityComponent &vel) {
//         // Limitar la velocidad en el eje X
//         if (vel.x > SPEED_LIMIT) vel.x = SPEED_LIMIT;
//         if (vel.x < -SPEED_LIMIT) vel.x = -SPEED_LIMIT;

//         // Limitar la velocidad en el eje Y
//         if (vel.y > SPEED_LIMIT) vel.y = SPEED_LIMIT;
//         if (vel.y < -SPEED_LIMIT) vel.y = -SPEED_LIMIT;
//     }
// };

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

class SpriteAnimationSystem : public UpdateSystem {
  void run(float dT) override {
    auto view = scene->r.view<SpriteComponent>();
    Uint32 now = SDL_GetTicks();

    for (auto e : view) {
      auto& spr = view.get<SpriteComponent>(e);

      if (spr.animationFrames > 0) {
        if (spr.lastUpdate == 0) {
          spr.lastUpdate = now;
          continue;
        }
        float timeSinceLastUpdate = now - spr.lastUpdate;

        int lastFrame = spr.animationFrames - 1;

        int framesToUpdate = timeSinceLastUpdate / spr.animationDuration * spr.animationFrames; 

        if (framesToUpdate > 0) {
          spr.xIndex += framesToUpdate;
          spr.xIndex %= spr.animationFrames;
          spr.lastUpdate = now;
        }

      }
    }
  }
};

class SpriteRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    auto view = scene->r.view<PositionComponent, SpriteComponent>();
    for (auto e : view) {
      auto pos = view.get<PositionComponent>(e);
      auto spr = view.get<SpriteComponent>(e);

      Texture* texture = TextureManager::GetTexture(spr.filename);
      SDL_Rect clip = { 
        spr.xIndex * spr.width, 
        spr.yIndex * spr.height,
        spr.width,
        spr.height 
      };

      texture->render(scene->renderer, pos.x, pos.y, spr.width * spr.scale, spr.height * spr.scale, &clip);
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

class BackgroundSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* background = scene->createEntity("Background");
    const std::string& bgfile = "assets/Background/Background.png";
    background->addComponent<TextureComponent>(bgfile);
    background->addComponent<BackgroundComponent>(bgfile);
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

class BackgroundRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer){
    auto view = scene->r.view<BackgroundComponent>();
    for (auto e : view) {
      auto tex = view.get<BackgroundComponent>(e);
      auto texture = TextureManager::GetTexture(tex.filename);
      texture->render(renderer, 0, 0);
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
      addSetupSystem<TextureSetupSystem>(sampleScene);

      addEventSystem<InputSystem>(sampleScene);
      addUpdateSystem<SpriteMovementSystem>(sampleScene);
      addUpdateSystem<MovementSystem>(sampleScene);
      addUpdateSystem<SpriteAnimationSystem>(sampleScene);
      addUpdateSystem<WallHitSystem>(sampleScene);
      

      addRenderSystem<BackgroundRenderSystem>(sampleScene);
      addRenderSystem<SpriteRenderSystem>(sampleScene);
      // addUpdateSystem<CollisionSystem>(sampleScene);

      setScene(sampleScene);
    }
}; 
