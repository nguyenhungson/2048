#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include "audio.h"
#include "events.h"
#include "game.h"
#include "graphics.h"
#include "globals.h"
#include "textures.h"

int main(int argc, char* argv[])
{
    // 1. Init SDL subsystems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << "\n";
        return 1;
    }
    if (TTF_Init() != 0) {
        std::cerr << "TTF init failed: " << TTF_GetError() << "\n";
        SDL_Quit();
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG) & (IMG_INIT_JPG|IMG_INIT_PNG))) {
        std::cerr << "SDL_image init failed: " << IMG_GetError() << "\n";
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! " << Mix_GetError() << "\n";
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // 2. Create window & renderer
    SDL_Window* window = SDL_CreateWindow("2048 Fruits",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1100, 700, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        Mix_CloseAudio();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // 3. Load audio & textures
    if (!initAudio()) {
        std::cerr << "Some audio files failed to load.\n";
        // You can choose to continue or quit
    }
    if (!loadAllTextures(renderer)) {
        std::cerr << "Some textures failed to load.\n";
    }

    // 4. Open fonts
    TTF_Font* titleFont   = TTF_OpenFont("doodle.ttf", 72);
    TTF_Font* smallFont   = TTF_OpenFont("doodle.ttf", 42);
    TTF_Font* buttonFont  = TTF_OpenFont("doodle.ttf", 36);
    if (!titleFont || !smallFont || !buttonFont) {
        std::cerr << "Font loading error: " << TTF_GetError() << "\n";
    }

    // 5. Recompute layout, load highscore, init game
    recomputeLayout(window);
    loadHighscore();
    // Optionally, start playing background music
    playBackgroundMusic(); // from audio.cpp

    // 6. Main loop
    bool running = true;
    while (running) {
        running = processEvents(window); // from events.cpp
        if (!running) {
            break; // user wants to quit
        }

        // Render the appropriate screen (using global flags from events)
        if (gameWon) {
            draw_win_screen(renderer, titleFont, smallFont);
        }
        else if (showOptions) {
            draw_options_screen(renderer, buttonFont, titleFont);
        }
        else if (showHelp) {
            draw_help_screen(renderer, titleFont, smallFont);
        }
        else if (showCredits) {
            draw_credits_screen(renderer, titleFont, smallFont, buttonFont);
        }
        else if (gameOver) {
            draw_game_over_screen(renderer, titleFont, smallFont);
        }
        else if (!gameStarted) {
            draw_start_screen(renderer);
        }
        else {
            draw_grid(renderer, smallFont);
        }
    }

    // 7. Cleanup
    TTF_CloseFont(titleFont);
    TTF_CloseFont(smallFont);
    TTF_CloseFont(buttonFont);

    freeAllTextures(); // from textures.cpp
    cleanupAudio();    // from audio.cpp

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}


