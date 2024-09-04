#pragma once
#include <string>
#include "Tilemap.h"

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

