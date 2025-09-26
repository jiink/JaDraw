#define SDL_MAIN_HANDLED
#include "JaDraw.h"
#include "TestSprite.h"
#include "luigi.h"
#include <SDL.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <span>
#include "system.h"
//#include "MyApplet.h"
//#include "ClockApplet.h"
//#include "SpaceGameApplet.h"
#include "SpaceGame3dApplet.h"
//#include "SnakeGameApplet.h"

const int CANVAS_WIDTH = WIDTH;
const int CANVAS_HEIGHT = HEIGHT;
const int SCREEN_SCALE = 2;
const int SCREEN_WIDTH = CANVAS_WIDTH * SCREEN_SCALE;
const int SCREEN_HEIGHT = CANVAS_HEIGHT * SCREEN_SCALE;

JaDraw<CANVAS_WIDTH, CANVAS_HEIGHT> jdrw;

int main(int argc, char* argv[]) {
    #if __cplusplus < 202002L
        #error "This code requires C++20 for std::span"
    #endif
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "JaDraw Canvas Viewer",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        CANVAS_WIDTH, CANVAS_HEIGHT
    );
    if (!texture) {
        std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool quit = false;
    SDL_Event e;
    float deltaTime = 0.016f;
    SpaceGame3dApplet applet;
    applet.setup();
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        // Handle input
        InputData input;
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_LEFT]) {
            input.rotation = -20;
        } else if (state[SDL_SCANCODE_RIGHT]) {
            input.rotation = 20;
        } else {
            input.rotation = 0;
        }
        input.pressed = state[SDL_SCANCODE_Z];

        applet.loop(jdrw, deltaTime, input);

        int pitch = CANVAS_WIDTH * sizeof(uint32_t);
        SDL_UpdateTexture(texture, NULL, jdrw.canvas.data(), pitch);

        SDL_SetRenderDrawColor(renderer, 0x33, 0x33, 0x33, 0xFF);
        SDL_RenderClear(renderer);

        SDL_Rect dstRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, texture, NULL, &dstRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}