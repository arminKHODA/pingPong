#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <sstream>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Paddle dimensions
const int PADDLE_WIDTH = 10;
const int PADDLE_HEIGHT = 50;

// Ball dimensions
const int BALL_SIZE = 10;

// Paddle speed
const float PADDLE_SPEED = 0.2f;

// Ball speed
const float BALL_SPEED = 0.2f;

void RenderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() < 0) {
        std::cerr << "Could not initialize SDL_ttf: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Ping Pong",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Could not create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("font.ttf", 24); // Provide the correct path to a TTF font file
    if (!font) {
        std::cerr << "Could not load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    float paddle1Y = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2.0f;
    float paddle2Y = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2.0f;
    float ballX = (SCREEN_WIDTH - BALL_SIZE) / 2.0f;
    float ballY = (SCREEN_HEIGHT - BALL_SIZE) / 2.0f;
    float ballVelX = BALL_SPEED;
    float ballVelY = BALL_SPEED;

    int score1 = 0;
    int score2 = 0;
    bool running = true;
    SDL_Event event;

    Uint32 lastTime = SDL_GetTicks();
    Uint32 currentTime, elapsedTime;

    while (running) {
        currentTime = SDL_GetTicks();
        elapsedTime = currentTime - lastTime;
        lastTime = currentTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_W] && paddle1Y > 0) {
            paddle1Y -= PADDLE_SPEED * elapsedTime;
        }
        if (state[SDL_SCANCODE_S] && paddle1Y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
            paddle1Y += PADDLE_SPEED * elapsedTime;
        }
        if (state[SDL_SCANCODE_UP] && paddle2Y > 0) {
            paddle2Y -= PADDLE_SPEED * elapsedTime;
        }
        if (state[SDL_SCANCODE_DOWN] && paddle2Y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
            paddle2Y += PADDLE_SPEED * elapsedTime;
        }

        ballX += ballVelX * elapsedTime;
        ballY += ballVelY * elapsedTime;

        if (ballY <= 0 || ballY >= SCREEN_HEIGHT - BALL_SIZE) {
            ballVelY = -ballVelY;
        }

        if (ballX <= PADDLE_WIDTH && ballY + BALL_SIZE > paddle1Y && ballY < paddle1Y + PADDLE_HEIGHT) {
            ballVelX = -ballVelX;
        }
        else if (ballX >= SCREEN_WIDTH - PADDLE_WIDTH - BALL_SIZE && ballY + BALL_SIZE > paddle2Y && ballY < paddle2Y + PADDLE_HEIGHT) {
            ballVelX = -ballVelX;
        }
        else if (ballX < 0) {
            score2++;
            ballX = (SCREEN_WIDTH - BALL_SIZE) / 2.0f;
            ballY = (SCREEN_HEIGHT - BALL_SIZE) / 2.0f;
            ballVelX = BALL_SPEED;
            ballVelY = BALL_SPEED;
        }
        else if (ballX > SCREEN_WIDTH - BALL_SIZE) {
            score1++;
            ballX = (SCREEN_WIDTH - BALL_SIZE) / 2.0f;
            ballY = (SCREEN_HEIGHT - BALL_SIZE) / 2.0f;
            ballVelX = -BALL_SPEED;
            ballVelY = -BALL_SPEED;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect paddle1 = { 0, static_cast<int>(paddle1Y), PADDLE_WIDTH, PADDLE_HEIGHT };
        SDL_Rect paddle2 = { SCREEN_WIDTH - PADDLE_WIDTH, static_cast<int>(paddle2Y), PADDLE_WIDTH, PADDLE_HEIGHT };
        SDL_Rect ball = { static_cast<int>(ballX), static_cast<int>(ballY), BALL_SIZE, BALL_SIZE };
        SDL_RenderFillRect(renderer, &paddle1);
        SDL_RenderFillRect(renderer, &paddle2);
        SDL_RenderFillRect(renderer, &ball);

        std::stringstream scoreText;
        scoreText << "Player 1: " << score1 << "  Player 2: " << score2;
        RenderText(renderer, font, scoreText.str(), SCREEN_WIDTH / 2 - 100, 10, { 255, 255, 255, 255 });

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
