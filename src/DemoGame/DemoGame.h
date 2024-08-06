#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include <printf.h>
#include <entt/entt.hpp>

#define WIDTH 1024
#define HEIGHT 768
#define BRICK_WIDTH 90
#define BRICK_HEIGHT 30
#define BRICK_SPACING 10

SDL_Color red = {0xFF, 0x00, 0x00, 0xFF};
SDL_Color orange = {0xFF, 0xA5, 0x00, 0xFF};
SDL_Color yellow = {0xFF, 0xFF, 0x00, 0xFF};
SDL_Color green = {0x00, 0xFF, 0x00, 0xFF};
SDL_Color blue = {0x00, 0x00, 0xFF, 0xFF};
SDL_Color purple = {0x80, 0x00, 0x80, 0xFF};
SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};


struct SpriteComponent {
  int width;
  int height;
  SDL_Color color;
};

class PaddleSpawnSetypSystem : public SetupSystem {
  void run() {

    Entity* paddle = scene->createEntity("PADDLE", 100, HEIGHT-110); 
    paddle->addComponent<VelocityComponent>(500, 500);
    paddle->addComponent<SpriteComponent>(150, 20, SDL_Color{255, 255, 255});
    paddle->addComponent<PlayerControlledComponent>();
  }
};


class SquareSpawnSetupSystem : public SetupSystem {
  void run() {
    

    int rows = 6;
    int cols = 10;
    SDL_Color colors[] = {red, orange, yellow, green, blue, purple};

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int posX = col * (BRICK_WIDTH + BRICK_SPACING) + WIDTH / 2 - (cols * (BRICK_WIDTH + BRICK_SPACING) / 2);
            int posY = row * (BRICK_HEIGHT + BRICK_SPACING) + 20;
            Entity* square = scene->createEntity("SQUARE2", posX, posY); 

            square->addComponent<SpriteComponent>(BRICK_WIDTH, BRICK_HEIGHT, orange);
        }
    }
    
  }
};

class BallCollisionSystem : public UpdateSystem {
  void run(float dT) {
    auto ballView = scene->r.view<PositionComponent, VelocityComponent, SpriteComponent>();
    auto paddleView = scene->r.view<PositionComponent, SpriteComponent, PlayerControlledComponent>();
    auto brickView = scene->r.view<PositionComponent, SpriteComponent>();

    for (auto ball : ballView) {
      auto& ballPos = ballView.get<PositionComponent>(ball);
      auto& ballVel = ballView.get<VelocityComponent>(ball);
      auto& ballSpr = ballView.get<SpriteComponent>(ball);

      // Detectar colisión con el paddle
      for (auto paddle : paddleView) {
        auto& paddlePos = paddleView.get<PositionComponent>(paddle);
        auto& paddleSpr = paddleView.get<SpriteComponent>(paddle);

        if (ballPos.x < paddlePos.x + paddleSpr.width &&
            ballPos.x + ballSpr.width > paddlePos.x &&
            ballPos.y < paddlePos.y + paddleSpr.height &&
            ballPos.y + ballSpr.height > paddlePos.y) {
          ballVel.y *= -1;
          ballPos.y = paddlePos.y - ballSpr.height; // Reubicar la bola encima del paddle
        }
      }

      // Detectar colisión con los ladrillos
      for (auto brick : brickView) {
        auto& brickPos = brickView.get<PositionComponent>(brick);
        auto& brickSpr = brickView.get<SpriteComponent>(brick);

        if (ballPos.x < brickPos.x + brickSpr.width &&
            ballPos.x + ballSpr.width > brickPos.x &&
            ballPos.y < brickPos.y + brickSpr.height &&
            ballPos.y + ballSpr.height > brickPos.y) {
          ballVel.y *= -1;
          scene->destroyEntity(brick); // Destruir el ladrillo
        }
      }
    }
  }
};


class BallSpawnSetupSystem : public SetupSystem {
  void run() {
    Entity* ball = scene->createEntity("BALL", WIDTH/2, HEIGHT/2); 
    ball->addComponent<VelocityComponent>(500, 500);
    ball->addComponent<SpriteComponent>(20, 20, SDL_Color{255, 255, 255});
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

      if (newPosY < 0 || newPosY + spr.height > 768) {
        vel.y *= -1.1;
        pos.y = 0 + (newPosY < 0 ? 0 : 768 - spr.height);
      }
      
    }
  }
};

class SquareRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    auto view = scene->r.view<PositionComponent, SpriteComponent>();
    for (auto e : view) {
      auto pos = view.get<PositionComponent>(e);
      auto spr = view.get<SpriteComponent>(e);

      SDL_SetRenderDrawColor(renderer, spr.color.r, spr.color.g, spr.color.b, spr.color.a);
      SDL_Rect r = { pos.x, pos.y, spr.width, spr.height };
      SDL_RenderFillRect(renderer, &r);
    }
  }
}; 

class InputSystem : public UpdateSystem {
  void run(float dT) {
    const Uint8* state = SDL_GetKeyboardState(NULL);

    auto view = scene->r.view<VelocityComponent, PlayerControlledComponent>();

    for (auto e : view) {
      auto& vel = view.get<VelocityComponent>(e);

      vel.x = 0;
      vel.y = 0;

      if (state[SDL_SCANCODE_A]) {
        vel.x = -500;
      }
      if (state[SDL_SCANCODE_D]) {
        vel.x = 500;
      }
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
      
      sampleScene = new Scene("BREAKOUT", r);
      addSetupSystem<PaddleSpawnSetypSystem>(sampleScene);
      addSetupSystem<BallSpawnSetupSystem>(sampleScene);
      addSetupSystem<SquareSpawnSetupSystem>(sampleScene);
      addUpdateSystem<MovementSystem>(sampleScene);
      addUpdateSystem<WallHitSystem>(sampleScene);
      addUpdateSystem<InputSystem>(sampleScene);
      addRenderSystem<SquareRenderSystem>(sampleScene);
      
      //addUpdateSystem<BallCollisionSystem>(sampleScene);

      setScene(sampleScene);
    }
}; 
