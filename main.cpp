#include <chrono>
#include <iostream>
#include <SDL.h>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr int MAX_FPS = 60;
constexpr int BALL_SPEED = 150;
constexpr int MAX_BALL_SPEED = 190;
constexpr int PADDLE_SPEED = 250;
constexpr int BALL_SIZE = 20;
constexpr int BRICK_WIDTH = 60;
constexpr int BRICK_HEIGHT = 20;
constexpr int BRICK_SPACING = 5;
constexpr int POWERUP_SIZE = 20;
constexpr int BALL_INITIAL_X = SCREEN_WIDTH / 2;
constexpr int BALL_INITIAL_Y = SCREEN_HEIGHT / 2;

char vidas = 5;
Uint32 lastUpdateTime = 0;

struct RectObject {
    SDL_Rect rect = {0, 0, 100, 100};
    int velocityX = 0;
    int velocityY = 0;
    SDL_Color color = {0xFF, 0xFF, 0xFF, 0xFF};
};

SDL_Color red = {0xFF, 0x00, 0x00, 0xFF};
SDL_Color orange = {0xFF, 0xA5, 0x00, 0xFF};
SDL_Color yellow = {0xFF, 0xFF, 0x00, 0xFF};
SDL_Color green = {0x00, 0xFF, 0x00, 0xFF};
SDL_Color blue = {0x00, 0x00, 0xFF, 0xFF};
SDL_Color purple = {0x80, 0x00, 0x80, 0xFF};
SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};

RectObject paddle = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT - 25, 100, 20}, 0, 0, white};
RectObject ballPrefab = {{BALL_INITIAL_X , BALL_INITIAL_Y, BALL_SIZE, BALL_SIZE}, -BALL_SPEED, BALL_SPEED, white};
RectObject powerUpPrefab = {{BALL_INITIAL_X , BALL_INITIAL_Y, POWERUP_SIZE, POWERUP_SIZE}, 0, 100, blue};

std::vector<RectObject> bricks;
std::vector<RectObject> balls = {ballPrefab};
std::vector<RectObject> powerUps = {};


SDL_Color getRandomColor() {
    SDL_Color colors[] = {red, orange, yellow, green, blue, purple, white};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(colors)/sizeof(colors[0]) - 1);
    return colors[dis(gen)];
}

void createBricks() {
    int rows = 6;
    int cols = 10;
    SDL_Color colors[] = {red, orange, yellow, green, blue, purple};

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int x = col * (BRICK_WIDTH + BRICK_SPACING);
            int y = row * (BRICK_HEIGHT + BRICK_SPACING);
            RectObject brick = {{x, y + 4, BRICK_WIDTH, BRICK_HEIGHT}, 0, 0, colors[row]};
            bricks.push_back(brick);
        }
    }
}

void renderRect(SDL_Renderer* renderer, RectObject& rect_object) {
    SDL_SetRenderDrawColor(renderer, rect_object.color.r, rect_object.color.g, rect_object.color.b, rect_object.color.a);
    SDL_RenderFillRect(renderer, &rect_object.rect);
}

bool checkCollision(const SDL_Rect&a, const SDL_Rect&b) {
    return (
        a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y);
}

void handleInput(SDL_Event& e) {
    // resolve
    /*
     * Todos los estados de teclado vienen en byte o en int8
     */
    const Uint8* ks = SDL_GetKeyboardState(NULL);

    paddle.velocityX = 0;
    paddle.velocityY = 0;

    if(ks[SDL_SCANCODE_LEFT] || ks[SDL_SCANCODE_A]) {
        paddle.velocityX = -PADDLE_SPEED;
    }

    if(ks[SDL_SCANCODE_RIGHT] || ks[SDL_SCANCODE_D]) {
        paddle.velocityX = PADDLE_SPEED;
    }
}

void createRandomPowerUp(int x, int y) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 99);
    if (dis(gen) < 20) {
        RectObject powerUp = {{x, y, POWERUP_SIZE, POWERUP_SIZE}, 0, 100, blue}; // Falling power-up
        powerUps.push_back(powerUp);
    }
}

void update(float dT) {

    if (paddle.rect.x < 0) {
        paddle.velocityX *= -1.1;
    }

    if (paddle.rect.y < 0) {
        paddle.velocityY *= -1.1;
    }

    if (paddle.rect.x + paddle.rect.w > SCREEN_WIDTH) {
        paddle.velocityX *= -1.1;
    }

    for (size_t i = 0; i < balls.size(); ++i) {
        if (balls[i].rect.x < 0) {
            balls[i].velocityX *= -1.1;
        }


        if (balls[i].rect.y < 0) {
            balls[i].velocityY *= -1.1;
        }


        if (balls[i].rect.x + balls[i].rect.w > SCREEN_WIDTH) {
            balls[i].velocityX *= -1.1;
        }

        if (balls[i].rect.y + balls[i].rect.h> SCREEN_HEIGHT) {
            balls.erase(balls.begin() + i);
            --i; // Ajustar el índice después de la eliminación
            continue;
        }

        // Colisión con la paleta
        if (checkCollision(balls[i].rect, paddle.rect)) {

            if (paddle.velocityY < MAX_BALL_SPEED) {
                balls[i].velocityY *= -1.1;
            }else {
                balls[i].velocityY *= -1;
            }

            if (paddle.velocityX < MAX_BALL_SPEED) {
                balls[i].velocityX = paddle.velocityX;
            }
            balls[i].rect.y = paddle.rect.y - balls[i].rect.h; // Evitar que la bola se quede pegada
        }

        // Colisiones con ladrillos
        for (size_t j = 0; j < bricks.size(); ++j) {
            if (checkCollision(balls[i].rect, bricks[j].rect)) {
                balls[i].velocityY *= -1.1;

                int brickCenterX = bricks[j].rect.x + bricks[j].rect.w / 2;
                int brickCenterY = bricks[j].rect.y + bricks[j].rect.h / 2;

                createRandomPowerUp(brickCenterX, brickCenterY);
                bricks.erase(bricks.begin() + j);
                --j; // Ajustar el índice después de la eliminación
            }
        }
        balls[i].rect.x +=  balls[i].velocityX * dT;
        balls[i].rect.y +=  balls[i].velocityY * dT;
    }

    for (size_t i = 0; i < powerUps.size(); ++i) {
        if (checkCollision(powerUps[i].rect, paddle.rect)) {
            powerUps.erase(powerUps.begin() + i);
            --i; // Ajustar el índice después de la eliminación
            balls.push_back(ballPrefab);
        }

        if (powerUps[i].rect.y > SCREEN_HEIGHT) {
            powerUps.erase(powerUps.begin() + i);
            --i; // Ajustar el índice después de la eliminación
            continue;
        }

        powerUps[i].rect.y += powerUps[i].velocityY * dT;
        powerUps[i].color = getRandomColor();
    }

    paddle.rect.x +=  paddle.velocityX * dT;
    paddle.rect.y +=  paddle.velocityY * dT;

    if(balls.empty() || bricks.empty()) {
        if(vidas == 0 || bricks.empty()) {
            exit(0);
        }
        vidas --;
        balls.push_back(ballPrefab);
    }
}

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    createBricks(); // Crear los ladrillos al inicio

    bool quit = false;
    SDL_Event e;

    int x = 0;
    int y = 0;

    Uint32 frameCount = 1;
    Uint32 frameStartTimeStamp = 1;
    Uint32 frameEndTimeStamp = 1;
    Uint32 lastFrameTime = SDL_GetTicks();
    constexpr float frameDuration = (1.0/MAX_FPS) * 1000.0;
    int FPS = MAX_FPS;

    while (!quit) {

        frameStartTimeStamp = SDL_GetTicks();

        // delta time
        Uint32 currentFrameTime = SDL_GetTicks();
        float dT = (currentFrameTime - lastFrameTime) / 1000.0;
        lastFrameTime = currentFrameTime;

        // poll events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            handleInput(e); // se llama por cada evento que suceda.
        }

        // update
        update(dT);

        // render
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        renderRect(renderer, paddle);
        for (RectObject ball: balls) {
            renderRect(renderer, ball);
        }
        for (RectObject brick: bricks) {
            renderRect(renderer, brick);
        }
        for (RectObject powerUp: powerUps) {
            renderRect(renderer, powerUp);
        }

        //renderRect(renderer, powerUp);

        SDL_RenderPresent(renderer);

        frameEndTimeStamp = SDL_GetTicks();
        float actualFrameDuration = frameEndTimeStamp - frameStartTimeStamp;

        if(actualFrameDuration < frameDuration) {
            SDL_Delay(frameDuration - actualFrameDuration); // para esperar un tiempo en un tipo IDLE. No consume CPU
        }

        // fps calculations
        frameCount ++;
        const Uint32 currentTime = SDL_GetTicks();
        const Uint32 elapsedTime = currentTime - lastUpdateTime;
        printf("Elapsed time: %d", elapsedTime);

        if (elapsedTime > 1000) {
            FPS = (float)frameCount / (elapsedTime / 1000.0);
            lastUpdateTime = currentTime;
            frameCount = 0;
        }

        SDL_SetWindowTitle(window, std::to_string(vidas).c_str());
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}