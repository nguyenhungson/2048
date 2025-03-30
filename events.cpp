// events.cpp
#include "boosters.h"
#include "globals.h"
#include "events.h"
#include "game.h"
#include "graphics.h"
#include "audio.h"
#include <SDL.h>
#include <iostream>

const int margin = 10;

bool processEvents(SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_Event e;
    bool quit = false;

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }
        else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            recomputeLayout(window);
        }
        else if (e.type == SDL_KEYDOWN) {
            std::cerr << "Key pressed: " << SDL_GetKeyName(e.key.keysym.sym) << std::endl;
            if (e.key.keysym.sym == SDLK_f) {
                if (!isFullscreen) {
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    isFullscreen = true;
                } else {
                    SDL_SetWindowFullscreen(window, 0);
                    isFullscreen = false;
                    recomputeLayout(window);
                }
            }
            else if (e.key.keysym.sym == SDLK_m) {
                SDL_MinimizeWindow(window);
            }
            else {
                if (!showOptions && !showHelp && !showCredits && !gameOver && !gameWon) {
                    if (!gameStarted) {
                        std::cerr << "Game is starting. Calling initialize_grid()." << std::endl;
                        gameStarted = true;
                        gameOver = false;
                        initialize_grid();
                    } else {
                        move_tiles(e.key.keysym.sym);
                        if (is_game_over()) {
                            std::cerr << "Game over detected after move." << std::endl;
                            gameOver = true;
                        }
                    }
                }
                // In menu mode.
                else {
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        std::cerr << "ESC pressed in menu. Returning to game." << std::endl;
                        showHelp = false;
                        showCredits = false;
                        showOptions = false;
                    }
                    if ((showHelp || showOptions || showCredits) &&
                        (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN)) {
                        if (e.key.keysym.sym == SDLK_UP) {
                            helpScrollOffset -= 20;
                            std::cerr << "Scrolling up. New helpScrollOffset: " << helpScrollOffset << std::endl;
                        } else {
                            helpScrollOffset += 20;
                            std::cerr << "Scrolling down. New helpScrollOffset: " << helpScrollOffset << std::endl;
                        }
                    }
                }
            }
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mouseX = e.button.x;
            int mouseY = e.button.y;
            std::cerr << "Mouse click at (" << mouseX << ", " << mouseY << ")" << std::endl;
            handleHammerBoosterClick(mouseX, mouseY, renderer);
            if (hammerActive && currentBoosterType == BOOSTER_HAMMER) {
                useHammerBoosterOnTile(mouseX, mouseY);
                continue;
            }
            handleFreezeBoosterClick(mouseX, mouseY, renderer);
            handleTsunamiBoosterClick(mouseX, mouseY, renderer);
            if (tsunamiActive && (currentBoosterType == BOOSTER_TSUNAMI)) {
                 useTsunamiBoosterOnTile(renderer);
                 continue;
            }
            if (!showOptions && !showHelp && !showCredits && !gameOver && !gameWon) {
                SDL_Rect optionButtonRect = { GAME_AREA_WIDTH + 10, WINDOW_HEIGHT - 60, SIDEBAR_WIDTH - 20, 50 };
                if (mouseX >= optionButtonRect.x && mouseX <= optionButtonRect.x + optionButtonRect.w &&
                    mouseY >= optionButtonRect.y && mouseY <= optionButtonRect.y + optionButtonRect.h) {
                    showOptions = true;
                    std::cerr << "Options button clicked." << std::endl;
                    continue;
                }
            }
            else if (showOptions) {
                const int sliderWidth = 300, sliderHeight = 20;
                int sliderX = (WINDOW_WIDTH - sliderWidth) / 2;
                const int baseY = 120, btnHeight = 70, spacing = 20;
                int musicSliderY = baseY + 4 * (btnHeight + spacing) + 40;
                int sfxSliderY = musicSliderY + 50;
                SDL_Rect musicSliderRect = { sliderX, musicSliderY, sliderWidth, sliderHeight };
                SDL_Rect sfxSliderRect = { sliderX, sfxSliderY, sliderWidth, sliderHeight };

                if (mouseX >= musicSliderRect.x && mouseX <= musicSliderRect.x + sliderWidth &&
                    mouseY >= musicSliderRect.y && mouseY <= musicSliderRect.y + sliderHeight) {
                    int clampedX = (mouseX < sliderX ? sliderX : (mouseX > sliderX + sliderWidth ? sliderX + sliderWidth : mouseX));
                    int sliderPos = clampedX - sliderX;
                    musicVolume = (sliderPos * DEFAULT_VOLUME) / sliderWidth;
                    setMusicVolume(musicVolume);
                    continue;
                }
                else if (mouseX >= sfxSliderRect.x && mouseX <= sfxSliderRect.x + sliderWidth &&
                         mouseY >= sfxSliderRect.y && mouseY <= sfxSliderRect.y + sliderHeight) {
                    int clampedX = (mouseX < sliderX ? sliderX : (mouseX > sliderX + sliderWidth ? sliderX + sliderWidth : mouseX));
                    int sliderPos = clampedX - sliderX;
                    sfxVolume = (sliderPos * DEFAULT_SFX_VOLUME) / sliderWidth;
                    setSFXVolume(sfxVolume);
                    continue;
                }

                const int btnWidth = 220, btnHeight2 = 70, spacing2 = 20;
                int startY2 = 120;
                SDL_Rect helpBtn       = { (WINDOW_WIDTH - btnWidth) / 2, startY2, btnWidth, btnHeight2 };
                SDL_Rect restartBtn    = { (WINDOW_WIDTH - btnWidth) / 2, startY2 + (btnHeight2 + spacing2), btnWidth, btnHeight2 };
                SDL_Rect creditBtn     = { (WINDOW_WIDTH - btnWidth) / 2, startY2 + 2 * (btnHeight2 + spacing2), btnWidth, btnHeight2 };
                SDL_Rect optionQuitBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY2 + 3 * (btnHeight2 + spacing2), btnWidth, btnHeight2 };
                SDL_Rect backBtn       = { WINDOW_WIDTH - btnWidth - 20, WINDOW_HEIGHT - btnHeight2 - 20, btnWidth, btnHeight2 };

                if (mouseX >= helpBtn.x && mouseX <= helpBtn.x + helpBtn.w &&
                    mouseY >= helpBtn.y && mouseY <= helpBtn.y + helpBtn.h) {
                    showHelp = true;
                    showOptions = false;
                    continue;
                }
                else if (mouseX >= restartBtn.x && mouseX <= restartBtn.x + restartBtn.w &&
                         mouseY >= restartBtn.y && mouseY <= restartBtn.y + restartBtn.h) {
                    initialize_grid();
                    incrementscore = 0;
                    gameStarted = true;
                    gameOver = false;
                    lock2048 = false;
                    gameWon = false;
                    showOptions = false;
                    continue;
                }
                else if (mouseX >= creditBtn.x && mouseX <= creditBtn.x + creditBtn.w &&
                         mouseY >= creditBtn.y && mouseY <= creditBtn.y + creditBtn.h) {
                    showCredits = true;
                    showOptions = false;
                    continue;
                }
                else if (mouseX >= optionQuitBtn.x && mouseX <= optionQuitBtn.x + optionQuitBtn.w &&
                         mouseY >= optionQuitBtn.y && mouseY <= optionQuitBtn.y + optionQuitBtn.h) {
                    quit = true;
                    continue;
                }
                else if (mouseX >= backBtn.x && mouseX <= backBtn.x + backBtn.w &&
                         mouseY >= backBtn.y && mouseY <= backBtn.y + backBtn.h) {
                    showOptions = false;
                    continue;
                }
            }
            else if (showHelp) {
                SDL_Rect closeButtonRect = { WINDOW_WIDTH - DEFAULT_CLOUD_BTN_WIDTH - 20,
                                             WINDOW_HEIGHT - DEFAULT_CLOUD_BTN_HEIGHT - 20,
                                             DEFAULT_CLOUD_BTN_WIDTH, DEFAULT_CLOUD_BTN_HEIGHT };
                if (mouseX >= closeButtonRect.x && mouseX <= closeButtonRect.x + closeButtonRect.w &&
                    mouseY >= closeButtonRect.y && mouseY <= closeButtonRect.y + closeButtonRect.h) {
                    showHelp = false;
                    showOptions = true;
                    continue;
                }
            }
            else if (showCredits) {
                SDL_Rect backButtonRect = { WINDOW_WIDTH - DEFAULT_CLOUD_BTN_WIDTH - 20,
                                            WINDOW_HEIGHT - DEFAULT_CLOUD_BTN_HEIGHT - 20,
                                            DEFAULT_CLOUD_BTN_WIDTH, DEFAULT_CLOUD_BTN_HEIGHT };
                if (mouseX >= backButtonRect.x && mouseX <= backButtonRect.x + backButtonRect.w &&
                    mouseY >= backButtonRect.y && mouseY <= backButtonRect.y + backButtonRect.h) {
                    showCredits = false;
                    showOptions = true;
                    continue;
                }
            }
            else if (gameOver) {
                const int btnWidth = 200;
                const int btnHeight = 60;
                const int spacing   = 20;
                int btnStartY = (WINDOW_HEIGHT / 4) + 100;
                int offset = 20;
                SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + offset, btnWidth, btnHeight };
                SDL_Rect quitBtn    = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight + spacing + offset, btnWidth, btnHeight };

                if (mouseX >= restartBtn.x && mouseX <= restartBtn.x + restartBtn.w &&
                    mouseY >= restartBtn.y && mouseY <= restartBtn.y + restartBtn.h) {
                    if (bgMusic)
                        Mix_PlayMusic(bgMusic, -1);
                    initialize_grid();
                    gameStarted = true;
                    gameOver = false;
                    lock2048 = false;
                    continue;
                }
                else if (mouseX >= quitBtn.x && mouseX <= quitBtn.x + quitBtn.w &&
                         mouseY >= quitBtn.y && mouseY <= quitBtn.y + quitBtn.h) {
                    quit = true;
                    continue;
                }
            }
            else if (gameWon) {
                const int btnWidth = 250, btnHeight = 70, spacing = 20;
                int btnStartY = (WINDOW_HEIGHT / 4) + 115;
                const int margin = 10;
                SDL_Rect continueBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight };
                SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight + spacing, btnWidth, btnHeight };

                if (mouseX >= continueBtn.x - margin && mouseX <= continueBtn.x + continueBtn.w + margin &&
                    mouseY >= continueBtn.y - margin && mouseY <= continueBtn.y + continueBtn.h + margin) {
                    lock2048 = true;
                    Mix_HaltMusic();
                    if (bgMusic)
                        Mix_PlayMusic(bgMusic, -1);
                    gameWon = false;
                    continue;
                }
                else if (mouseX >= quitBtn.x - margin && mouseX <= quitBtn.x + quitBtn.w + margin &&
                         mouseY >= quitBtn.y - margin && mouseY <= quitBtn.y + quitBtn.h + margin) {
                    quit = true;
                    continue;
                }
            }
        }
        else if (showOptions && e.type == SDL_MOUSEMOTION && (e.motion.state & SDL_BUTTON_LMASK)) {
            const int sliderWidth = 300, sliderHeight = 20;
            int sliderX = (WINDOW_WIDTH - sliderWidth) / 2;
            const int baseY = 120, btnHeight = 70, spacing = 20;
            int musicSliderY = baseY + 4 * (btnHeight + spacing) + 40;
            int sfxSliderY = musicSliderY + 50;
            SDL_Rect musicSliderRect = { sliderX, musicSliderY, sliderWidth, sliderHeight };
            if (e.motion.x >= musicSliderRect.x && e.motion.x <= musicSliderRect.x + sliderWidth &&
                e.motion.y >= musicSliderRect.y && e.motion.y <= musicSliderRect.y + sliderHeight) {
                int clampedX = (e.motion.x < sliderX ? sliderX : (e.motion.x > sliderX + sliderWidth ? sliderX + sliderWidth : e.motion.x));
                int sliderPos = clampedX - sliderX;
                musicVolume = (sliderPos * DEFAULT_VOLUME) / sliderWidth;
                Mix_VolumeMusic(musicVolume);
            }
            SDL_Rect sfxSliderRect = { sliderX, sfxSliderY, sliderWidth, sliderHeight };
            if (e.motion.x >= sfxSliderRect.x && e.motion.x <= sfxSliderRect.x + sliderWidth &&
                e.motion.y >= sfxSliderRect.y && e.motion.y <= sfxSliderRect.y + sliderHeight) {
                int clampedX = (e.motion.x < sliderX ? sliderX : (e.motion.x > sliderX + sliderWidth ? sliderX + sliderWidth : e.motion.x));
                int sliderPos = clampedX - sliderX;
                sfxVolume = (sliderPos * DEFAULT_SFX_VOLUME) / sliderWidth;
                Mix_VolumeChunk(swipeSound, sfxVolume);
            }
        }
    }
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

    return !quit;
}
