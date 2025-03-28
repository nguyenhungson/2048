#include "globals.h"
#include "events.h"
#include "audio.h"      // so we can call setMusicVolume, setSFXVolume, etc.
#include "game.h"       // for initialize_grid(), move_tiles(), is_game_over(), etc.
#include "graphics.h"   // possibly for recomputeLayout if that’s in graphics
#include "textures.h"   // if you need to refer to some texture data
#include <SDL.h>

// We assume these come from some "globals" or are declared extern in events.h
bool gameStarted     = false;
bool gameOver        = false;
bool gameWon         = false;
bool showOptions     = false;
bool showHelp        = false;
bool showCredits     = false;
bool isFullscreen    = false;

bool musicSliderActive = false;
bool sfxSliderActive   = false;
int helpScrollOffset   = 0;

bool processEvents(SDL_Window* window)
{
    SDL_Event e;
    bool quit = false;  // Ensure 'quit' is declared

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return false;  // Signal to quit
        }
        else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            recomputeLayout(window);
        }
        else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_f) {
                if (!isFullscreen) {
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                } else {
                    SDL_SetWindowFullscreen(window, 0);
                    recomputeLayout(window);
                }
                isFullscreen = !isFullscreen;
            }
            else if (e.key.keysym.sym == SDLK_m) {
                SDL_MinimizeWindow(window);
            }
            else if (e.key.keysym.sym == SDLK_ESCAPE) {
                showHelp = showCredits = showOptions = false;
            }
            else if (!showOptions && !showHelp && !showCredits && !gameOver && !gameWon) {
                if (!gameStarted) {
                    gameStarted = true;
                    gameOver = false;
                    initialize_grid();
                } else {
                    move_tiles(e.key.keysym.sym);
                    if (is_game_over()) gameOver = true;
                }
            }
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mouseX = e.button.x, mouseY = e.button.y;

            if (showOptions) {
                SDL_Rect optionQuitBtn = { 100, 200, 220, 70 }; // Example position
                if (mouseX >= optionQuitBtn.x && mouseX <= optionQuitBtn.x + optionQuitBtn.w &&
                    mouseY >= optionQuitBtn.y && mouseY <= optionQuitBtn.y + optionQuitBtn.h) {
                    quit = true;
                }
            }
        }
    }

    return !quit;  // Return true if still running
}
