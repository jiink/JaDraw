// AI slop to test it out.

// --- START OF FILE main.cpp ---

#define SDL_MAIN_HANDLED
#include "JaDraw.h" // Your drawing class header
#include <SDL.h>     // SDL main header
#include <iostream>
#include <vector>   // For potential resizing/scaling if needed later
#include <stdexcept> // For exception handling

// Define the size of the canvas (using the template)
constexpr int CANVAS_WIDTH = 64; // Let's make it a bit bigger
constexpr int CANVAS_HEIGHT = 48;

// --- SDL Specific Setup ---
const int SCREEN_SCALE = 10; // Scale up the small canvas for visibility
const int SCREEN_WIDTH = CANVAS_WIDTH * SCREEN_SCALE;
const int SCREEN_HEIGHT = CANVAS_HEIGHT * SCREEN_SCALE;

// Our statically allocated drawing canvas instance
JaDraw<CANVAS_WIDTH, CANVAS_HEIGHT> jdrw;

int main(int argc, char* argv[]) {
    // --- SDL Initialization ---
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

    // Create an SDL texture that we will update with our canvas data
    // SDL_PIXELFORMAT_ARGB8888 is often a good match for uint32_t color format
    // (Alpha, Red, Green, Blue - check SDL docs if colors seem swapped, might need ABGR etc.)
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888, // Or SDL_PIXELFORMAT_RGBA8888 etc. depending on uint32_t mapping
        SDL_TEXTUREACCESS_STREAMING, // We'll be updating it frequently from CPU
        CANVAS_WIDTH, CANVAS_HEIGHT
    );
    if (!texture) {
        std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // --- Initial Drawing Example ---
    // Draw something on the canvas before the loop starts
    jdrw.drawPixel(0, 0, 0xFFFF0000); // Red pixel at top-left (ARGB format assumed)
    jdrw.drawPixel(CANVAS_WIDTH - 1, 0, 0xFF00FF00); // Green at top-right
    jdrw.drawPixel(0, CANVAS_HEIGHT - 1, 0xFF0000FF); // Blue at bottom-left
    jdrw.drawPixel(CANVAS_WIDTH - 1, CANVAS_HEIGHT - 1, 0xFFFFFFFF); // White at bottom-right
    // Draw a line
    for(int i = 0; i < CANVAS_WIDTH / 2; ++i) {
        jdrw.drawPixel(i, i, 0xFFFFFF00); // Yellow diagonal
    }


    // --- Main Loop ---
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            // Example: Draw a pixel on mouse click
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                // Scale mouse coords back to canvas coords
                int canvasX = mouseX / SCREEN_SCALE;
                int canvasY = mouseY / SCREEN_SCALE;
                jdrw.drawPixel(canvasX, canvasY, 0xFFFFA500); // Draw Orange pixel
            }
        }

        // --- Update the Texture ---
        // Copy the raw pixel data from our JaDraw canvas to the SDL texture
        // The 'pitch' is the number of bytes per row in our source data.
        int pitch = CANVAS_WIDTH * sizeof(uint32_t);
        SDL_UpdateTexture(texture, NULL, jdrw.canvas.data(), pitch);

        // --- Render ---
        SDL_SetRenderDrawColor(renderer, 0x33, 0x33, 0x33, 0xFF); // Background color (dark grey)
        SDL_RenderClear(renderer);

        // Copy the texture to the rendering target, scaling it up
        SDL_Rect dstRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, texture, NULL, &dstRect); // NULL srcRect means use whole texture

        // Update screen
        SDL_RenderPresent(renderer);
    }

    // --- Cleanup ---
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// --- END OF FILE main.cpp ---