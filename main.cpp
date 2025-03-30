#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include "audio.h"
#include "boosters.h"
#include "events.h"
#include "font.h"
#include "game.h"
#include "graphics.h"
#include "globals.h"
#include "textures.h"

int main(int argc, char* argv[])
{
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

    window = SDL_CreateWindow("2048 Fruits",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1100, 700, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        Mix_CloseAudio();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    renderer = SDL_CreateRenderer(window, -1,
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

    if (!initAudio()) {
        std::cerr << "Some audio files failed to load.\n";
    }
    if (!loadAllTextures(renderer)) {
        std::cerr << "Some textures failed to load.\n";
    }
    if (!initFont()){
        std::cerr << "Some fonts failed to load.\n";
    }

    recomputeLayout(window);
    loadHighscore();
    loadBoosterTextures(renderer);

    bool running = true;
    while (running) {
        running = processEvents(window, renderer);
        if (boosterActive) {
            Uint32 elapsed = SDL_GetTicks() - boosterStartTime;
            if (elapsed >= currentBooster.duration) {
                boosterActive = false;
                std::cerr << "Booster expired." << std::endl;
            }
        }
        if (!running) {
            break;
        }
    }

    freeAllFont();
    freeAllTextures();
    cleanupAudio();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}


