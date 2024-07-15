#include <chrono>
#include <iostream>
#include <SDL.h>
#include <string>

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr int MAX_FPS = 60.0f;
constexpr int BALL_SPEED = 10;
Uint32 lastUpdateTime = 0;

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event e;

    int x = 0;
    int y = 0;

    Uint32 frameCount = 1;
    Uint32 frameStartTimeStamp = 1;
    Uint32 frameEndTimeStamp = 1;
    constexpr float frameDuration = (1.0/MAX_FPS) * 1000.0;
    int FPS = MAX_FPS;

    while (!quit) {

        frameStartTimeStamp = SDL_GetTicks();

        // poll events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // update
        if (x < SCREEN_WIDTH) {
            x = x + BALL_SPEED; // pixeles por frame
        }

        if(y < SCREEN_HEIGHT) {
            y = y + BALL_SPEED;
        }

        // render
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        SDL_Rect fillRect = {x, y, 100, 100};
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, &fillRect);

        SDL_RenderPresent(renderer);

        frameEndTimeStamp = SDL_GetTicks();
        float actualFrameDuration = frameEndTimeStamp - frameStartTimeStamp;

        if(actualFrameDuration < frameDuration) {
            SDL_Delay(frameDuration - actualFrameDuration); // para esperar un tiempo en un tipo IDLE. No consume CPU
        }

        // fps calculations
        frameCount ++;
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - lastUpdateTime;
        printf("Elapsed time: %d", elapsedTime);

        if (elapsedTime > 1000) {
            FPS = (float)frameCount / (elapsedTime / 1000.0);
            lastUpdateTime = currentTime;
            frameCount = 0;
        }

        SDL_SetWindowTitle(window, std::to_string(FPS).c_str());
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}