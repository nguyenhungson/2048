#include "boosters.h"
#include "graphics.h"
#include "globals.h"
#include "game.h"
#include "textures.h"
#include "font.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>

void recomputeLayout(SDL_Window* window)
{
    int actualW, actualH;
    SDL_GetWindowSize(window, &actualW, &actualH);
    int proposedGameWidth = (actualW * 7) / 8;
    if (proposedGameWidth > actualH)
        proposedGameWidth = actualH;
    int tileSize = proposedGameWidth / GRID_SIZE;
    GAME_AREA_WIDTH = tileSize * GRID_SIZE;
    SIDEBAR_WIDTH = actualW - GAME_AREA_WIDTH;
    WINDOW_WIDTH = actualW;
    WINDOW_HEIGHT = actualH;
    TILE_SIZE = tileSize;
    if (SIDEBAR_WIDTH < 0) {
        SIDEBAR_WIDTH = 0;
        GAME_AREA_WIDTH = actualW;
        TILE_SIZE = actualW / GRID_SIZE;
        std::cerr << "Warning: Not enough width for sidebar, using full width for grid.\n";
    }
}

void draw_start_screen(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);

    if (startBackground) {
        SDL_Rect destRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderCopy(renderer, startBackground, nullptr, &destRect);
    }

    SDL_RenderPresent(renderer);
}

void draw_grid(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    if (gridBackground) {
        SDL_Rect destRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderCopy(renderer, gridBackground, NULL, &destRect);
    }
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int value = grid[i][j];
            SDL_Rect rect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            if (value == BLOCKER_VALUE) {
                if (blockerTexture) {
                    SDL_RenderCopy(renderer, blockerTexture, NULL, &rect);
                }
            }
            else if (value != 0 && fruitTextures.count(value)) {
                SDL_Texture* tex = fruitTextures[value];
                SDL_Rect destRect = { rect.x, rect.y, TILE_SIZE, TILE_SIZE };
                SDL_RenderCopy(renderer, tex, NULL, &destRect);
            }
        }
    }
    draw_sidebar(renderer, valueFont, smallFont);
    SDL_RenderPresent(renderer);
}

void draw_sidebar(SDL_Renderer* renderer, TTF_Font* valueFont, TTF_Font* smallFont) {
    SDL_Rect sidebarRect = { GAME_AREA_WIDTH, 0, SIDEBAR_WIDTH, WINDOW_HEIGHT };
    if (sidebarBackground) {
        SDL_RenderCopy(renderer, sidebarBackground, nullptr, &sidebarRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        SDL_RenderFillRect(renderer, &sidebarRect);
    }

    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Color incrementColor = {0, 255, 0, 255};

    std::string scoreTitle = "Score";
    SDL_Surface* scoreTitleSurface = TTF_RenderText_Solid(smallFont, scoreTitle.c_str(), textColor);
    if (scoreTitleSurface) {
        SDL_Texture* scoreTitleTexture = SDL_CreateTextureFromSurface(renderer, scoreTitleSurface);
        int titleX = GAME_AREA_WIDTH + (SIDEBAR_WIDTH - scoreTitleSurface->w) / 2;
        int titleY = 70;
        SDL_Rect titleRect = { titleX, titleY, scoreTitleSurface->w, scoreTitleSurface->h };
        SDL_RenderCopy(renderer, scoreTitleTexture, nullptr, &titleRect);
        SDL_FreeSurface(scoreTitleSurface);
        SDL_DestroyTexture(scoreTitleTexture);
    }

    if (scoreBackground) {
        SDL_Rect scoreBgRect;
        scoreBgRect.w = 300;
        scoreBgRect.h = 70;
        scoreBgRect.x = GAME_AREA_WIDTH + (SIDEBAR_WIDTH - scoreBgRect.w) / 2;
        scoreBgRect.y = 120;
        SDL_RenderCopy(renderer, scoreBackground, nullptr, &scoreBgRect);

        if (incrementscore > 0) {
            SDL_Surface* scoreSurface = TTF_RenderText_Solid(smallFont, std::to_string(score).c_str(), textColor);
            SDL_Surface* incrementSurface = TTF_RenderText_Solid(smallFont, (" + " + std::to_string(incrementscore)).c_str(), incrementColor);
            if (scoreSurface && incrementSurface) {
                const int spacing = 0;
                int totalWidth = scoreSurface->w + spacing + incrementSurface->w;
                int startX = scoreBgRect.x + (scoreBgRect.w - totalWidth) / 2;
                int scoreY = scoreBgRect.y + (scoreBgRect.h - scoreSurface->h) / 2;
                int incrementY = scoreBgRect.y + (scoreBgRect.h - incrementSurface->h) / 2;

                SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                SDL_Texture* incrementTexture = SDL_CreateTextureFromSurface(renderer, incrementSurface);
                SDL_FreeSurface(scoreSurface);
                SDL_FreeSurface(incrementSurface);

                SDL_Rect scoreRect = { startX, scoreY, 0, 0 };
                SDL_QueryTexture(scoreTexture, NULL, NULL, &scoreRect.w, &scoreRect.h);
                SDL_Rect incrementRect = { startX + scoreRect.w + spacing, incrementY, 0, 0 };
                SDL_QueryTexture(incrementTexture, NULL, NULL, &incrementRect.w, &incrementRect.h);

                SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
                SDL_RenderCopy(renderer, incrementTexture, nullptr, &incrementRect);

                SDL_DestroyTexture(scoreTexture);
                SDL_DestroyTexture(incrementTexture);
            }
        } else {
            SDL_Surface* scoreSurface = TTF_RenderText_Solid(smallFont, std::to_string(score).c_str(), textColor);
            if (scoreSurface) {
                SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                SDL_Rect scoreRect = {
                    scoreBgRect.x + (scoreBgRect.w - scoreSurface->w) / 2,
                    scoreBgRect.y + (scoreBgRect.h - scoreSurface->h) / 2,
                    scoreSurface->w,
                    scoreSurface->h
                };
                SDL_FreeSurface(scoreSurface);
                SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
                SDL_DestroyTexture(scoreTexture);
            }
        }
    }

    std::string highTitle = "Highscore";
    SDL_Surface* highTitleSurface = TTF_RenderText_Solid(smallFont, highTitle.c_str(), textColor);
    if (highTitleSurface) {
        SDL_Texture* highTitleTexture = SDL_CreateTextureFromSurface(renderer, highTitleSurface);
        int highTitleX = GAME_AREA_WIDTH + (SIDEBAR_WIDTH - highTitleSurface->w) / 2;
        int highTitleY = 200;
        SDL_Rect highTitleRect = { highTitleX, highTitleY, highTitleSurface->w, highTitleSurface->h };
        SDL_RenderCopy(renderer, highTitleTexture, nullptr, &highTitleRect);
        SDL_FreeSurface(highTitleSurface);
        SDL_DestroyTexture(highTitleTexture);
    }

    if (scoreBackground) {
        SDL_Rect highBgRect;
        highBgRect.w = 300;
        highBgRect.h = 70;
        highBgRect.x = GAME_AREA_WIDTH + (SIDEBAR_WIDTH - highBgRect.w) / 2;
        highBgRect.y = 250;
        SDL_RenderCopy(renderer, scoreBackground, nullptr, &highBgRect);

        std::string highscoreText = std::to_string(highscore);
        SDL_Surface* highSurface = TTF_RenderText_Solid(smallFont, highscoreText.c_str(), textColor);
        if (highSurface) {
            SDL_Texture* highTexture = SDL_CreateTextureFromSurface(renderer, highSurface);
            SDL_Rect highRect;
            highRect.w = highSurface->w;
            highRect.h = highSurface->h;
            highRect.x = highBgRect.x + (highBgRect.w - highSurface->w) / 2;
            highRect.y = highBgRect.y + (highBgRect.h - highSurface->h) / 2;
            SDL_RenderCopy(renderer, highTexture, nullptr, &highRect);
            SDL_FreeSurface(highSurface);
            SDL_DestroyTexture(highTexture);
        }
    }
    if (boosterActive) {
    // Calculate remaining time in the booster period.
    Uint32 elapsed = SDL_GetTicks() - boosterStartTime;
    Uint32 remaining = (elapsed < BOOSTER_DURATION) ? (BOOSTER_DURATION - elapsed) : 0;
    // Calculate the width of the booster bar.
    int fullBarWidth = 300;  // full width of the bar in pixels
    int currentBarWidth = static_cast<int>((remaining / (float)BOOSTER_DURATION) * fullBarWidth);

    // Define the booster bar rectangle.
    SDL_Rect boosterBarRect = {
        GAME_AREA_WIDTH + (SIDEBAR_WIDTH - fullBarWidth) / 2, // center horizontally in sidebar
        400,    // vertical position (adjust as needed)
        currentBarWidth,
        30      // height of the bar
    };
    // Draw the filled part of the booster bar.
    SDL_SetRenderDrawColor(renderer, 0, 225, 0, 255);
    SDL_RenderFillRect(renderer, &boosterBarRect);
    // Draw the border of the booster bar.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &boosterBarRect);

    double secondsRemaining = remaining / 1000.0;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << secondsRemaining;
    std::string timeStr = oss.str();

    std::string boosterLabel = "Booster Active! x" + std::to_string(currentBooster.multiplier) +
                               " (" + timeStr + "s)";
    SDL_Surface* boosterLabelSurf = TTF_RenderText_Solid(boosterFont, boosterLabel.c_str(), textColor);
    if (boosterLabelSurf) {
        SDL_Texture* boosterLabelTex = SDL_CreateTextureFromSurface(renderer, boosterLabelSurf);
        // Center the booster label above the booster bar.
        SDL_Rect boosterLabelRect = {
            GAME_AREA_WIDTH + (SIDEBAR_WIDTH - boosterLabelSurf->w) / 2,
            boosterBarRect.y - boosterLabelSurf->h - 5,
            boosterLabelSurf->w,
            boosterLabelSurf->h
        };
        SDL_FreeSurface(boosterLabelSurf);
        SDL_RenderCopy(renderer, boosterLabelTex, nullptr, &boosterLabelRect);
        SDL_DestroyTexture(boosterLabelTex);
    }
}
    if (newHighscoreAchieved && recordBackground) {
        SDL_Rect congratsRect;
        congratsRect.w = 250;
        congratsRect.h = 150;
        congratsRect.x = GAME_AREA_WIDTH + (SIDEBAR_WIDTH - congratsRect.w) / 2;
        congratsRect.y = WINDOW_HEIGHT - 200;
        SDL_RenderCopy(renderer, recordBackground, nullptr, &congratsRect);

        std::string congratsMsg = "Congratulations!\nNew Record!";
        SDL_Surface* congratsSurface = TTF_RenderText_Blended_Wrapped(valueFont, congratsMsg.c_str(), textColor, congratsRect.w - 10);
        if (congratsSurface) {
            SDL_Texture* congratsTexture = SDL_CreateTextureFromSurface(renderer, congratsSurface);
            SDL_Rect congratsTextRect;
            congratsTextRect.w = congratsSurface->w;
            congratsTextRect.h = congratsSurface->h;
            congratsTextRect.x = congratsRect.x + (congratsRect.w - congratsSurface->w) / 2 + 50;
            congratsTextRect.y = congratsRect.y + (congratsRect.h - congratsSurface->h) / 2;
            SDL_RenderCopy(renderer, congratsTexture, nullptr, &congratsTextRect);
            SDL_FreeSurface(congratsSurface);
            SDL_DestroyTexture(congratsTexture);
        }
        if (newHighscoreAchieved && (SDL_GetTicks() - newHighscoreTime >= 5000)) {
            newHighscoreAchieved = false;
        }
    }
    drawBoosterIcons(renderer);
    if (freezeActive) {
        drawFreezeBoosterDuration(renderer, boosterFont);
    }
    SDL_Rect optionButtonRect = { GAME_AREA_WIDTH + 10, WINDOW_HEIGHT - 60, SIDEBAR_WIDTH - 20, 50 };
    drawCloudButtonWithText(renderer, cloudTexture, optionButtonRect, "Options", smallFont);

    SDL_RenderPresent(renderer);
}


void draw_help_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont)
{
    SDL_RenderClear(renderer);
    if (optionBackground) {
        SDL_Rect destRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, optionBackground, NULL, &destRect);
    }
    SDL_Color textColor = {0, 0, 0, 255};
    std::vector<std::pair<std::string, TTF_Font*>> lines;

    lines.push_back({ "Welcome to 2048 Fruits!", titleFont });
    lines.push_back({ "", smallFont }); // Empty line for spacing

    // Objective Section
    lines.push_back({ "* Objective:", smallFont });
    lines.push_back({ "Merge matching fruit tiles by sliding them", smallFont });
    lines.push_back({ "with the arrow keys. The goal is to reach", smallFont });
    lines.push_back({ "the highest possible score.", smallFont });
    lines.push_back({ "", smallFont });

    // How to Play Section
    lines.push_back({ "* How to Play:", smallFont });
    lines.push_back({ "- Use arrow keys (Up, Down, Left, Right) to slide tiles.", smallFont });
    lines.push_back({ "- Identical tiles merge to form higher values.", smallFont });
    lines.push_back({ "- Game ends when no moves remain.", smallFont });
    lines.push_back({ "- Highscore is saved automatically.", smallFont });
    lines.push_back({ "", smallFont });

    // Boosters Section
    lines.push_back({ "* Boosters:", smallFont });
    lines.push_back({ "- Hammer: Destroy any tile instantly.", smallFont });
    lines.push_back({ "- Freeze: Temporarily stop blockers production.", smallFont });
    lines.push_back({ "- Tsunami: Wipe all tiles.", smallFont });
    lines.push_back({ "", smallFont });

    // Fruit to Points Section
    lines.push_back({ "* Fruit to Points:", smallFont });
    lines.push_back({ "- Apple: 2 points", smallFont });
    lines.push_back({ "- Banana: 4 points", smallFont });
    lines.push_back({ "- Grape: 8 points", smallFont });
    lines.push_back({ "- Mango: 16 points", smallFont });
    lines.push_back({ "- Orange: 32 points", smallFont });
    lines.push_back({ "- Peach: 64 points", smallFont });
    lines.push_back({ "- Pineapple: 128 points", smallFont });
    lines.push_back({ "- Watermelon: 256 points", smallFont });
    lines.push_back({ "- Strawberry: 512 points", smallFont });
    lines.push_back({ "- Cherry: 1024 points", smallFont });
    lines.push_back({ "- Coconut: 2048 points", smallFont });
    lines.push_back({ "", smallFont });

    int totalHeight = 0;
    for (auto &line : lines) {
        totalHeight += TTF_FontLineSkip(line.second);
    }
    if (helpScrollOffset < 0)
        helpScrollOffset = 0;
    if (helpScrollOffset > totalHeight - WINDOW_HEIGHT)
        helpScrollOffset = totalHeight - WINDOW_HEIGHT;
    int y = 10 - helpScrollOffset;
    for (auto &line : lines) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(line.second, line.first.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = { (WINDOW_WIDTH - textSurface->w) / 2, y, textSurface->w, textSurface->h };
            y += TTF_FontLineSkip(line.second);
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
    }

    SDL_Rect closeButtonRect = { WINDOW_WIDTH - DEFAULT_CLOUD_BTN_WIDTH - 20, WINDOW_HEIGHT - DEFAULT_CLOUD_BTN_HEIGHT - 20, DEFAULT_CLOUD_BTN_WIDTH, DEFAULT_CLOUD_BTN_HEIGHT };
    drawCloudButtonWithText(renderer, cloudTexture, closeButtonRect, "Close", smallFont);

    SDL_RenderPresent(renderer);
}

void draw_credits_screen(SDL_Renderer* renderer,
                         TTF_Font* titleFont,
                         TTF_Font* smallFont,
                         TTF_Font* buttonFont)
{
    SDL_RenderClear(renderer);
    if (optionBackground) {
        SDL_Rect bgRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, optionBackground, NULL, &bgRect);
    }
    SDL_Color textColor = {0, 0, 0, 255};

    std::string creditsTitle = "Credits";
    SDL_Surface* titleSurface = TTF_RenderText_Solid(titleFont, creditsTitle.c_str(), textColor);
    if (titleSurface) {
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
        SDL_Rect titleRect = { (WINDOW_WIDTH - titleSurface->w) / 2, 20, titleSurface->w, titleSurface->h };
        SDL_FreeSurface(titleSurface);
        SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
        SDL_DestroyTexture(titleTexture);
    }

    std::string creditsText = "Developed by MVPSON98 HARDSTUCK SILVER\n\n\nPROPS TO DATSKII FOR THE LOVELY ARTWORK";
    std::istringstream iss(creditsText);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    int y = 120;
    for (const auto &l : lines) {
        SDL_Surface* lineSurface = TTF_RenderText_Solid(smallFont, l.c_str(), textColor);
        if (lineSurface) {
            SDL_Texture* lineTexture = SDL_CreateTextureFromSurface(renderer, lineSurface);
            SDL_Rect lineRect = { (WINDOW_WIDTH - lineSurface->w) / 2, y, lineSurface->w, lineSurface->h };
            y += lineSurface->h + 5;
            SDL_FreeSurface(lineSurface);
            SDL_RenderCopy(renderer, lineTexture, NULL, &lineRect);
            SDL_DestroyTexture(lineTexture);
        }
    }

    SDL_Rect backBtn = { WINDOW_WIDTH - DEFAULT_CLOUD_BTN_WIDTH - 20, WINDOW_HEIGHT - DEFAULT_CLOUD_BTN_HEIGHT - 20, DEFAULT_CLOUD_BTN_WIDTH, DEFAULT_CLOUD_BTN_HEIGHT };
    drawCloudButtonWithText(renderer, cloudTexture, backBtn, "Back", buttonFont);

    SDL_RenderPresent(renderer);
}

void draw_options_screen(SDL_Renderer* renderer,
                         TTF_Font* buttonFont,
                         TTF_Font* titleFont)
{
    SDL_RenderClear(renderer);
    if (optionBackground) {
        SDL_Rect bgRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, optionBackground, NULL, &bgRect);
    }

    SDL_Color textColor = {0, 0, 0, 255};

    SDL_Surface* titleSurf = TTF_RenderText_Solid(titleFont, "Options", textColor);
    if (titleSurf) {
        SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurf);
        SDL_Rect titleRect = { (WINDOW_WIDTH - titleSurf->w) / 2, 20, titleSurf->w, titleSurf->h };
        SDL_FreeSurface(titleSurf);
        SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
        SDL_DestroyTexture(titleTex);
    }

    const int CLOUD_BTN_WIDTH = 220;
    const int CLOUD_BTN_HEIGHT = 70;
    const int spacing = 20;
    int startY = 120;

    SDL_Rect helpBtn = { (WINDOW_WIDTH - CLOUD_BTN_WIDTH) / 2, startY, CLOUD_BTN_WIDTH, CLOUD_BTN_HEIGHT };
    SDL_Rect restartBtn = { (WINDOW_WIDTH - CLOUD_BTN_WIDTH) / 2, startY + CLOUD_BTN_HEIGHT + spacing, CLOUD_BTN_WIDTH, CLOUD_BTN_HEIGHT };
    SDL_Rect creditBtn = { (WINDOW_WIDTH - CLOUD_BTN_WIDTH) / 2, startY + 2 * (CLOUD_BTN_HEIGHT + spacing), CLOUD_BTN_WIDTH, CLOUD_BTN_HEIGHT };
    SDL_Rect optionQuitBtn = { (WINDOW_WIDTH - CLOUD_BTN_WIDTH) / 2, startY + 3 * (CLOUD_BTN_HEIGHT + spacing), CLOUD_BTN_WIDTH, CLOUD_BTN_HEIGHT };

    SDL_Rect backBtn = { WINDOW_WIDTH - CLOUD_BTN_WIDTH - 20, WINDOW_HEIGHT - CLOUD_BTN_HEIGHT - 20, CLOUD_BTN_WIDTH, CLOUD_BTN_HEIGHT };

    auto drawCloudButton = [&](SDL_Rect btnRect, const char* text) {
        drawCloudButtonWithText(renderer, cloudTexture, btnRect, text, buttonFont);
    };

    drawCloudButton(helpBtn, "Help");
    drawCloudButton(restartBtn, "Restart");
    drawCloudButton(creditBtn, "Credits");
    drawCloudButton(optionQuitBtn, "Quit");

    const int sliderWidth = 300;
    const int sliderHeight = 20;
    const int toggleSize = sliderHeight + 10;

    int musicSliderY = startY + 4 * (CLOUD_BTN_HEIGHT + spacing) + 30;
    SDL_Rect musicSliderBg = { (WINDOW_WIDTH - sliderWidth) / 2, musicSliderY, sliderWidth, sliderHeight };

    int sfxSliderY = musicSliderY + 60;
    SDL_Rect sfxSliderBg = { (WINDOW_WIDTH - sliderWidth) / 2, sfxSliderY, sliderWidth, sliderHeight };

    if (musicbarTexture) {
        SDL_RenderCopy(renderer, musicbarTexture, NULL, &musicSliderBg);
    } else {
        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
        SDL_RenderFillRect(renderer, &musicSliderBg);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &musicSliderBg);
    }
    int sliderX = musicSliderBg.x;
    int musicToggleX = sliderX + (musicVolume * (sliderWidth - toggleSize)) / DEFAULT_VOLUME;
    SDL_Rect musicToggleRect = { musicToggleX, musicSliderY - 5, toggleSize, toggleSize };
    if (musictoggleTexture) {
        SDL_RenderCopy(renderer, musictoggleTexture, NULL, &musicToggleRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 250, 255);
        SDL_RenderFillRect(renderer, &musicToggleRect);
    }

    SDL_Surface* musicLabelSurf = TTF_RenderText_Solid(buttonFont, "Music", textColor);
    if (musicLabelSurf) {
        SDL_Texture* musicLabelTex = SDL_CreateTextureFromSurface(renderer, musicLabelSurf);
        SDL_Rect musicLabelRect = {
            sliderX + (sliderWidth - musicLabelSurf->w) / 2,
            musicSliderY - 35,
            musicLabelSurf->w,
            musicLabelSurf->h
        };
        SDL_FreeSurface(musicLabelSurf);
        SDL_RenderCopy(renderer, musicLabelTex, NULL, &musicLabelRect);
        SDL_DestroyTexture(musicLabelTex);
    }

    if (musicbarTexture) {
        SDL_RenderCopy(renderer, musicbarTexture, NULL, &sfxSliderBg);
    } else {
        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
        SDL_RenderFillRect(renderer, &sfxSliderBg);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &sfxSliderBg);
    }
    int sfxToggleX = sfxSliderBg.x + (sfxVolume * (sliderWidth - toggleSize)) / DEFAULT_SFX_VOLUME;
    SDL_Rect sfxToggleRect = { sfxToggleX, sfxSliderBg.y - 5, toggleSize, toggleSize };
    if (musictoggleTexture) {
        SDL_RenderCopy(renderer, musictoggleTexture, NULL, &sfxToggleRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 250, 255);
        SDL_RenderFillRect(renderer, &sfxToggleRect);
    }

    SDL_Surface* sfxLabelSurf = TTF_RenderText_Solid(buttonFont, "SFX", textColor);
    if (sfxLabelSurf) {
        SDL_Texture* sfxLabelTex = SDL_CreateTextureFromSurface(renderer, sfxLabelSurf);
        SDL_Rect sfxLabelRect = {
            sfxSliderBg.x + (sliderWidth - sfxLabelSurf->w) / 2,
            sfxSliderBg.y - 35,
            sfxLabelSurf->w,
            sfxLabelSurf->h
        };
        SDL_FreeSurface(sfxLabelSurf);
        SDL_RenderCopy(renderer, sfxLabelTex, NULL, &sfxLabelRect);
        SDL_DestroyTexture(sfxLabelTex);
    }

    drawCloudButton(backBtn, "Back");

    SDL_RenderPresent(renderer);
}

void draw_game_over_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont)
{
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);

    if (optionBackground) {
        SDL_Rect bgRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderCopy(renderer, optionBackground, NULL, &bgRect);
    }

    if (currentGameoverIndex != 0 && gameoverTextures.count(currentGameoverIndex)) {
        SDL_Rect sideRect;
        sideRect.w = WINDOW_WIDTH / 4;
        sideRect.h = WINDOW_HEIGHT / 3 + 50;
        sideRect.x = WINDOW_WIDTH - sideRect.w - 50;
        sideRect.y = WINDOW_HEIGHT / 3 - 20;
        SDL_RenderCopy(renderer, gameoverTextures[currentGameoverIndex], nullptr, &sideRect);
    }

    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(titleFont, "Game Over!", textColor);
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
    SDL_Rect gameOverRect = {
        (WINDOW_WIDTH - gameOverSurface->w) / 2,
        (WINDOW_HEIGHT / 4) - (gameOverSurface->h / 2),
        gameOverSurface->w,
        gameOverSurface->h
    };
    SDL_FreeSurface(gameOverSurface);
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
    SDL_DestroyTexture(gameOverTexture);

    std::string resultText = "Your Score: " + std::to_string(score);
    SDL_Surface* resultSurface = TTF_RenderText_Solid(smallFont, resultText.c_str(), textColor);
    SDL_Texture* resultTexture = SDL_CreateTextureFromSurface(renderer, resultSurface);
    SDL_Rect resultRect = {
        (WINDOW_WIDTH - resultSurface->w) / 2,
        gameOverRect.y + gameOverRect.h + 10,
        resultSurface->w,
        resultSurface->h
    };
    SDL_FreeSurface(resultSurface);
    SDL_RenderCopy(renderer, resultTexture, NULL, &resultRect);
    SDL_DestroyTexture(resultTexture);

    const int btnWidth = 200, btnHeight = 60, spacing = 20;
    int btnStartY = resultRect.y + resultRect.h + 30;
    SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight };
    SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight + spacing, btnWidth, btnHeight };

    drawCloudButtonWithText(renderer, cloudTexture, restartBtn, "Restart", smallFont);
    drawCloudButtonWithText(renderer, cloudTexture, quitBtn, "Quit", smallFont);

    SDL_RenderPresent(renderer);
}


void draw_win_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont)
{
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);

    if (gamewonBackground) {
        SDL_Rect bgRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderCopy(renderer, gamewonBackground, NULL, &bgRect);
    }
    else if (optionBackground) {
        SDL_Rect bgRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderCopy(renderer, optionBackground, NULL, &bgRect);
    }

    if (!gamewinTextures.empty()) {
        SDL_Rect sideRect;
        sideRect.w = WINDOW_WIDTH / 4;
        sideRect.h = WINDOW_HEIGHT / 3;
        sideRect.x = WINDOW_WIDTH - sideRect.w - 50;
        sideRect.y = WINDOW_HEIGHT / 3;
        if (gamewinTextures.count(currentWinIndex))
            SDL_RenderCopy(renderer, gamewinTextures[currentWinIndex], NULL, &sideRect);
    }

    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface* winSurface = TTF_RenderText_Solid(titleFont, "Congratulations!", textColor);
    SDL_Texture* winTexture = SDL_CreateTextureFromSurface(renderer, winSurface);
    SDL_Rect winRect = {
        (WINDOW_WIDTH - winSurface->w) / 2,
        (WINDOW_HEIGHT / 4) - (winSurface->h / 2),
        winSurface->w,
        winSurface->h
    };
    SDL_FreeSurface(winSurface);
    SDL_RenderCopy(renderer, winTexture, NULL, &winRect);
    SDL_DestroyTexture(winTexture);

    std::string winLine = "You reached 2048!";
    SDL_Surface* winLineSurf = TTF_RenderText_Solid(smallFont, winLine.c_str(), textColor);
    SDL_Texture* winLineTex = SDL_CreateTextureFromSurface(renderer, winLineSurf);
    SDL_Rect winLineRect = {
        (WINDOW_WIDTH - winLineSurf->w) / 2,
        winRect.y + winRect.h + 10,
        winLineSurf->w,
        winLineSurf->h
    };
    SDL_FreeSurface(winLineSurf);
    SDL_RenderCopy(renderer, winLineTex, NULL, &winLineRect);
    SDL_DestroyTexture(winLineTex);

    const int btnWidth = 250, btnHeight = 70, spacing = 20;
    int btnStartY = winLineRect.y + winLineRect.h + 30;
    SDL_Rect continueBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight };
    SDL_Rect quitBtn     = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + (btnHeight + spacing), btnWidth, btnHeight };

    drawCloudButtonWithText(renderer, cloudTexture, continueBtn, "Continue", smallFont);
    drawCloudButtonWithText(renderer, cloudTexture, quitBtn, "Quit", smallFont);

    SDL_RenderPresent(renderer);
}

void drawCloudButtonWithText(SDL_Renderer* renderer, SDL_Texture* cloudTex, const SDL_Rect &btnRect, const char* text, TTF_Font* font) {
    if (cloudTex) {
        SDL_RenderCopy(renderer, cloudTex, NULL, &btnRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 250, 100, 100, 255);
        SDL_RenderFillRect(renderer, &btnRect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &btnRect);
    }
    SDL_Color blackColor = {0, 0, 0, 255};
    SDL_Surface* textSurf = TTF_RenderText_Solid(font, text, blackColor);
    if (textSurf) {
        SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
        SDL_Rect textRect = {
            btnRect.x + (btnRect.w - textSurf->w) / 2,
            btnRect.y + (btnRect.h - textSurf->h) / 2,
            textSurf->w,
            textSurf->h
        };
        SDL_FreeSurface(textSurf);
        SDL_RenderCopy(renderer, textTex, NULL, &textRect);
        SDL_DestroyTexture(textTex);
    }
}


