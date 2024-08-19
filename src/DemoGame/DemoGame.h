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


struct SpriteComponent {
  std::string texturePath;
  int width;
  int height;
  SDL_Color color;
};

class PaddleSpawnSetypSystem : public SetupSystem {
  void run() {

    Entity* paddle = scene->createEntity("PADDLE", WIDTH / 2, HEIGHT-110); 
    paddle->addComponent<VelocityComponent>(500, 500);
    paddle->addComponent<SpriteComponent>(PADDLE_WIDTH, PADDLE_HEIGHT, SDL_Color{255, 255, 255});
    paddle->addComponent<PlayerControlledComponent>();
  }
};


class SquareSpawnSetupSystem : public SetupSystem {
  void run() {
    
    int brickCount = 0;
    int rows = 6;
    int cols = 10;
    SDL_Color colors[] = {red, orange, yellow, green, blue, purple};

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int posX = col * (BRICK_WIDTH + BRICK_SPACING) + WIDTH / 2 - (cols * (BRICK_WIDTH + BRICK_SPACING) / 2);
            int posY = row * (BRICK_HEIGHT + BRICK_SPACING) + 20;
            Entity* square = scene->createEntity("SQUARE2", posX, posY); 

            square->addComponent<SpriteComponent>(BRICK_WIDTH, BRICK_HEIGHT, orange);
            square->addComponent<BrickComponent>();
            brickCount++;
        }
    }

    scene->setBrickCount(brickCount);
  }
};

class BallSpawnSetupSystem : public SetupSystem {
  void run() {
    Entity* ball = scene->createEntity("BALL", WIDTH/2, HEIGHT/2); 
    ball->addComponent<VelocityComponent>(250, 250);
    ball->addComponent<SpriteComponent>(20, 20, SDL_Color{255, 255, 255});
    ball->addComponent<BallComponent>();
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

class CollisionSystem : public UpdateSystem {
public:
    void run(float dT) override {
        auto view = scene->r.view<PositionComponent, VelocityComponent>();

        if (scene->getBrickCount() == 0) {
          printf("============================================\n");
          printf("¡Has ganado el juego!\n");
          printf("============================================\n");
          exit(0);
        }

        for (auto entity : view) {
            auto &pos = view.get<PositionComponent>(entity);
            auto &vel = view.get<VelocityComponent>(entity);

            if (scene->r.any_of<BallComponent>(entity)) {

                limitSpeed(vel);
                
                auto paddleView = scene->r.view<PositionComponent, PlayerControlledComponent>();
                for (auto paddleEntity : paddleView) {
                    auto &paddlePos = paddleView.get<PositionComponent>(paddleEntity);
                    if (checkCollision(pos, paddlePos, PADDLE_WIDTH, PADDLE_HEIGHT)) { // Paddle size (width=50, height=10)
                        // Invertir la dirección de la pelota
                        vel.y = -vel.y;
                    }
                }

                // Colisión con los bricks
                auto brickView = scene->r.view<PositionComponent, BrickComponent>();
                for (auto brickEntity : brickView) {
                    auto &brickPos = brickView.get<PositionComponent>(brickEntity);
                    if (checkCollision(pos, brickPos, BRICK_WIDTH, BRICK_HEIGHT)) {
                        // Colisión detectada, eliminar el bloque
                        scene->destroyEntity(brickEntity);
                        scene->decreaseBrickCount();

                        // Invertir la dirección de la pelota
                        vel.y = -vel.y;
                        break;
                    }
                }
            }
        }
    }

    void setScene(Scene *s) {
        scene = s;
    }

private:
    Scene *scene;

    bool checkCollision(const PositionComponent &a, const PositionComponent &b, int width, int height) {
        // Implementar lógica de detección de colisiones
        return !(a.x + width < b.x ||
                 a.x > b.x + width ||
                 a.y + height < b.y ||
                 a.y > b.y + height);
    }

    void limitSpeed(VelocityComponent &vel) {
        // Limitar la velocidad en el eje X
        if (vel.x > SPEED_LIMIT) vel.x = SPEED_LIMIT;
        if (vel.x < -SPEED_LIMIT) vel.x = -SPEED_LIMIT;

        // Limitar la velocidad en el eje Y
        if (vel.y > SPEED_LIMIT) vel.y = SPEED_LIMIT;
        if (vel.y < -SPEED_LIMIT) vel.y = -SPEED_LIMIT;
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

      // Si la bola toca el suelo se acaba el juego y se imprime un mensaje
      if (newPosY + spr.height > 768) {
        printf("============================================\n");
        std::printf("GAME OVER\n");
        printf("============================================\n");
        exit(0);
      }
      
      if(newPosY < 0) {
        vel.y *= -1.1;
        pos.y = 0;
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

struct TextureComponent {
  std::string filename;
};

struct BackgroundComponent {
  std::string filename;
};

class BackgroundSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* background = scene->createEntity("Background");
    const std::string& bgfile = "assets/stars.jpeg";
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
      
      sampleScene = new Scene("BREAKOUT", r, renderer);
      addSetupSystem<PaddleSpawnSetypSystem>(sampleScene);
      addSetupSystem<BallSpawnSetupSystem>(sampleScene);
      addSetupSystem<SquareSpawnSetupSystem>(sampleScene);

      addSetupSystem<BackgroundSetupSystem>(sampleScene);
      addSetupSystem<TextureSetupSystem>(sampleScene);
      addRenderSystem<BackgroundRenderSystem>(sampleScene);

      addUpdateSystem<MovementSystem>(sampleScene);
      addUpdateSystem<WallHitSystem>(sampleScene);
      addUpdateSystem<InputSystem>(sampleScene);
      addRenderSystem<SquareRenderSystem>(sampleScene);
      addUpdateSystem<CollisionSystem>(sampleScene);

      setScene(sampleScene);
    }
}; 
