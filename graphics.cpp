#include "graphics.h"
#include "globals.h"
#include "game.h"      // for grid, score, etc.
#include "textures.h"  // for background textures, fruit textures
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>

// If you store your textures in textures.cpp, you might need “extern” references here
extern SDL_Texture* startBackground;
extern SDL_Texture* gridBackground;
extern SDL_Texture* optionBackground;
extern SDL_Texture* cloudTexture;
extern std::map<int, SDL_Texture*> fruitTextures;

// Global layout variables (declared extern somewhere):
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern int GAME_AREA_WIDTH;
extern int SIDEBAR_WIDTH;
extern int TILE_SIZE;

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
        SDL_Rect destRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, startBackground, nullptr, &destRect);
    }
    SDL_RenderPresent(renderer);
}

void draw_sidebar(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Rect sidebarRect = { GAME_AREA_WIDTH, 0, SIDEBAR_WIDTH, WINDOW_HEIGHT };
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderFillRect(renderer, &sidebarRect);
    SDL_Color textColor = {0, 0, 0, 255};

    std::string scoreText = "Score: " + std::to_string(score);
    if (incrementscore)
        scoreText += " + " + std::to_string(incrementscore);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = { GAME_AREA_WIDTH + 10, 50, scoreSurface->w, scoreSurface->h };
    SDL_FreeSurface(scoreSurface);
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_DestroyTexture(scoreTexture);

    std::string highscoreText = "Highscore: " + std::to_string(highscore);
    SDL_Surface* highSurface = TTF_RenderText_Solid(font, highscoreText.c_str(), textColor);
    SDL_Texture* highTexture = SDL_CreateTextureFromSurface(renderer, highSurface);
    SDL_Rect highRect = { GAME_AREA_WIDTH + 10, 100, highSurface->w, highSurface->h };
    SDL_FreeSurface(highSurface);
    SDL_RenderCopy(renderer, highTexture, NULL, &highRect);
    SDL_DestroyTexture(highTexture);

    if (newHighscoreAchieved) {
        std::string congratsMsg = "Congratulations!";
        SDL_Surface* congratsSurface = TTF_RenderText_Solid(font, congratsMsg.c_str(), textColor);
        if (congratsSurface) {
            SDL_Texture* congratsTexture = SDL_CreateTextureFromSurface(renderer, congratsSurface);
            SDL_Rect congratsRect = { GAME_AREA_WIDTH + 10, 100 + congratsSurface->h + 10,
                                      congratsSurface->w, congratsSurface->h };
            SDL_FreeSurface(congratsSurface);
            SDL_RenderCopy(renderer, congratsTexture, NULL, &congratsRect);
            SDL_DestroyTexture(congratsTexture);
        }
    }

    SDL_Rect optionButtonRect = { GAME_AREA_WIDTH + 10, WINDOW_HEIGHT - 60, SIDEBAR_WIDTH - 20, 50 };
    drawCloudButtonWithText(renderer, cloudTexture, optionButtonRect, "Options", font);

    SDL_RenderPresent(renderer);
}

void draw_grid(SDL_Renderer* renderer, TTF_Font* font)
{
    // Clear screen, draw gridBackground if any, then draw the tiles
    // Then draw sidebar
    // Then SDL_RenderPresentSDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    if (gridBackground) {
        SDL_Rect destRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderCopy(renderer, gridBackground, NULL, &destRect);
    }
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int value = grid[i][j];
            SDL_Rect rect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            if (value != 0 && fruitTextures.count(value)) {
                SDL_Texture* tex = fruitTextures[value];
                SDL_Rect destRect = { rect.x, rect.y, TILE_SIZE, TILE_SIZE };
                SDL_RenderCopy(renderer, tex, NULL, &destRect);
            }
        }
    }
    draw_sidebar(renderer, font);
    SDL_RenderPresent(renderer);
}

void draw_help_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont)
{
    // Clear, draw optionBackground, draw instructions, draw “Close” as cloud
    // SDL_RenderPresentSDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    if (optionBackground) {
        SDL_Rect destRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, optionBackground, NULL, &destRect);
    }
    SDL_Color textColor = {0, 0, 0, 255};

    // Render title (scrolls with the paragraph).
    std::vector<std::pair<std::string, TTF_Font*>> lines;
    lines.push_back({ "Welcome to 2048 Fruits!", titleFont });
    lines.push_back({ "", smallFont });
    lines.push_back({ "* Objective:", smallFont });
    lines.push_back({ "Combine matching fruit tiles by sliding them", smallFont });
    lines.push_back({ "with the arrow keys. When two tiles with the", smallFont });
    lines.push_back({ "same value collide, they merge into one with", smallFont });
    lines.push_back({ "double the points. Reach the 2048 tile while", smallFont });
    lines.push_back({ "achieving the highest score.", smallFont });
    lines.push_back({ "", smallFont });
    lines.push_back({ "* How to Play:", smallFont });
    lines.push_back({ "- Use arrow keys to move tiles.", smallFont });
    lines.push_back({ "- Identical tiles merge to double points.", smallFont });
    lines.push_back({ "- Game over when no moves remain.", smallFont });
    lines.push_back({ "- Highscore is saved persistently.", smallFont });
    lines.push_back({ "", smallFont });
    lines.push_back({ "* Fruit to Points:", smallFont });
    lines.push_back({ "- Apple: 2", smallFont });
    lines.push_back({ "- Banana: 4", smallFont });
    lines.push_back({ "- Dragonfruit: 8", smallFont });
    lines.push_back({ "- Grape: 16", smallFont });
    lines.push_back({ "- Mango: 32", smallFont });
    lines.push_back({ "- Orange: 64", smallFont });
    lines.push_back({ "- Peach: 128", smallFont });
    lines.push_back({ "- Pineapple: 256", smallFont });
    lines.push_back({ "- Pomegranate: 512", smallFont });
    lines.push_back({ "- Strawberry: 1024", smallFont });
    lines.push_back({ "- Watermelon: 2048", smallFont });
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
    // Clear, draw optionBackground, draw “Credits” text, draw back cloud
    // SDL_RenderPresentSDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
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

    std::string creditsText = "Developed by MVPSON98 HARDSTUCK SILVER\n\nPROPS TO DATSKII FOR THE LOVELY ARTWORK";
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
    // Clear, draw optionBackground, draw 4 cloud buttons (Help, Restart, Credits, Quit)
    // Draw volume sliders
    // Draw back button
    // SDL_RenderPresentSDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
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
    // Clear, “Game Over!”, “Your Score: x”, 2 buttons (Restart, Quit)
    // SDL_RenderPresentSDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
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

    const int btnWidth = 200, btnHeight = 50, spacing = 20;
    int btnStartY = resultRect.y + resultRect.h + 30;
    SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight };
    SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight + spacing, btnWidth, btnHeight };

    drawCloudButtonWithText(renderer, cloudTexture, restartBtn, "Restart", smallFont);
    drawCloudButtonWithText(renderer, cloudTexture, quitBtn, "Quit", smallFont);

    SDL_RenderPresent(renderer);
}

void draw_win_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont)
{
    // Clear, “Congratulations!”, “Continue / Restart / Quit” buttons
    // SDL_RenderPresentSDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
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

    const int btnWidth = 200, btnHeight = 50, spacing = 20;
    int btnStartY = winLineRect.y + winLineRect.h + 30;
    SDL_Rect continueBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight };
    SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight + spacing, btnWidth, btnHeight };
    SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + 2 * (btnHeight + spacing), btnWidth, btnHeight };

    drawCloudButtonWithText(renderer, cloudTexture, continueBtn, "Continue", smallFont);
    drawCloudButtonWithText(renderer, cloudTexture, restartBtn, "Restart", smallFont);
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
