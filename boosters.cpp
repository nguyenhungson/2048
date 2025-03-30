#include "boosters.h"
#include "globals.h"
#include "game.h"
#include "graphics.h"
#include "font.h"
#include "audio.h"
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <iomanip>

Mix_Chunk* hammerSound = nullptr;
Mix_Chunk* freezeSound = nullptr;
Mix_Chunk* tsunamiSound = nullptr;

bool hammerActive = false;
bool freezeActive = false;
bool tsunamiActive = false;
Uint32 freezeStartTime = 0;

// Global booster button definitions.
BoosterButton hammerButton = { BOOSTER_HAMMER, 300, nullptr, nullptr };
BoosterButton freezeButton = { BOOSTER_FREEZE, 1500, nullptr, nullptr };
BoosterButton tsunamiButton = { BOOSTER_TSUNAMI, 3000, nullptr, nullptr };
BoosterType currentBoosterType = BOOSTER_HAMMER;

bool loadBoosterTextures(SDL_Renderer* renderer)
{
    SDL_Surface* hammerbuttonSurface = IMG_Load("assets/backgrounds and textures/hammerbutton.png");
    if (!hammerbuttonSurface){
        std::cerr << "Failed to load hammer button surface: " << IMG_GetError() << "\n";
    }
    else{
        hammerButton.iconTexture = SDL_CreateTextureFromSurface(renderer, hammerbuttonSurface);
        if (!hammerButton.iconTexture){
            std::cerr << "Failed to load hammer button texture" << IMG_GetError() << "\n";
        }
        SDL_FreeSurface(hammerbuttonSurface);
    }
    SDL_Surface* freezebuttonSurface = IMG_Load("assets/backgrounds and textures/freezebutton.png");
    if (!freezebuttonSurface){
        std::cerr << "Failed to load freeze button surface: " << IMG_GetError() << "\n";
    }
    else{
        freezeButton.iconTexture = SDL_CreateTextureFromSurface(renderer, freezebuttonSurface);
        if (!freezeButton.iconTexture){
            std::cerr << "Failed to load freeze button texture" << IMG_GetError() << "\n";
        }
        SDL_FreeSurface(freezebuttonSurface);
    }
    SDL_Surface* tsunamibuttonSurface = IMG_Load("assets/backgrounds and textures/tsunamibutton.png");
    if (!tsunamibuttonSurface){
        std::cerr << "Failed to load tsunami button surface: " << IMG_GetError() << "\n";
    }
    else{
        tsunamiButton.iconTexture = SDL_CreateTextureFromSurface(renderer, tsunamibuttonSurface);
        if (!tsunamiButton.iconTexture){
            std::cerr << "Failed to load tsunami button texture" << IMG_GetError() << "\n";
        }
        SDL_FreeSurface(tsunamibuttonSurface);
    }
    return true;
}

void freeBoosterTextures(SDL_Renderer* renderer)
{
    if (hammerButton.iconTexture) { SDL_DestroyTexture(hammerButton.iconTexture); hammerButton.iconTexture = nullptr; }
    if (hammerButton.cursorTexture) { SDL_DestroyTexture(hammerButton.cursorTexture); hammerButton.cursorTexture = nullptr; }
    if (freezeButton.iconTexture) { SDL_DestroyTexture(freezeButton.iconTexture); freezeButton.iconTexture = nullptr; }
    if (tsunamiButton.iconTexture) { SDL_DestroyTexture(tsunamiButton.iconTexture); tsunamiButton.iconTexture = nullptr; }
}

void drawBoosterIcons(SDL_Renderer* renderer)
{
    const int iconSize = 50;
    const int spacing = 20;

    int startX = GAME_AREA_WIDTH + SIDEBAR_WIDTH / 4;
    int startY = WINDOW_HEIGHT - 6 * spacing;

    SDL_Rect hammerRect  = { startX, startY, iconSize, iconSize };
    SDL_Rect freezeRect  = { startX + iconSize + spacing, startY, iconSize, iconSize };
    SDL_Rect tsunamiRect = { startX + 2 * (iconSize + spacing), startY, iconSize, iconSize };

    if (hammerButton.iconTexture) {
        SDL_RenderCopy(renderer, hammerButton.iconTexture, nullptr, &hammerRect);
    } else {
        std::cerr << "Hammer icon texture not loaded!" << std::endl;
    }

    if (freezeButton.iconTexture) {
        SDL_RenderCopy(renderer, freezeButton.iconTexture, nullptr, &freezeRect);
    } else {
        std::cerr << "Freeze icon texture not loaded!" << std::endl;
    }

    if (tsunamiButton.iconTexture) {
        SDL_RenderCopy(renderer, tsunamiButton.iconTexture, nullptr, &tsunamiRect);
    } else {
        std::cerr << "Tsunami icon texture not loaded!" << std::endl;
    }
}

SDL_Cursor* setBoosterCursor(SDL_Renderer* renderer, BoosterButton button)
{
    std::string cursorPath;
    if (button.type == BOOSTER_HAMMER) {
        cursorPath = "assets/backgrounds and textures/hammercursor.png";
    }
    SDL_Surface* cursorSurface = IMG_Load(cursorPath.c_str());
    if (!cursorSurface) {
        std::cerr << "Failed to load booster cursor for type " << button.type
                  << " (" << cursorPath << "): " << IMG_GetError() << "\n";
        return SDL_GetDefaultCursor();
    }
    int hotX = cursorSurface->w / 2;
    int hotY = cursorSurface->h / 2;
    SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, hotX, hotY);
    SDL_FreeSurface(cursorSurface);
    return cursor;
}

void handleHammerBoosterClick(int mouseX, int mouseY, SDL_Renderer* renderer)
{
    const int HAMMER_ICON_X = GAME_AREA_WIDTH + SIDEBAR_WIDTH / 4;
    const int HAMMER_ICON_Y = WINDOW_HEIGHT - 6 * 20;
    const int HAMMER_ICON_W = 50;
    const int HAMMER_ICON_H = 50;
    SDL_Rect hammerIconRect = { HAMMER_ICON_X, HAMMER_ICON_Y, HAMMER_ICON_W, HAMMER_ICON_H };

    if (mouseX >= hammerIconRect.x && mouseX <= hammerIconRect.x + hammerIconRect.w &&
        mouseY >= hammerIconRect.y && mouseY <= hammerIconRect.y + hammerIconRect.h)
    {
        // Check if the player has enough score.
        if (score >= hammerButton.cost) {
            score -= hammerButton.cost;
            std::cerr << "Deducted " << hammerButton.cost << " points. New score: " << score << std::endl;
            currentBoosterType = BOOSTER_HAMMER;
            hammerActive = true;
            SDL_Cursor* newCursor = setBoosterCursor(renderer, hammerButton);
            SDL_SetCursor(newCursor);
            std::cerr << "Hammer booster activated." << std::endl;
        } else {
            std::cerr << "Not enough score for hammer booster. Current score: " << score << std::endl;
            // Optionally, ensure the cursor is reset to the default.
            SDL_SetCursor(SDL_GetDefaultCursor());
        }
    }
}

void handleFreezeBoosterClick(int mouseX, int mouseY, SDL_Renderer* renderer)
{
    const int ICON_SIZE = 50;
    const int SPACING = 20;
    int startX = GAME_AREA_WIDTH + SIDEBAR_WIDTH / 4;
    int startY = WINDOW_HEIGHT - 6 * 20;
    SDL_Rect freezeIconRect = { startX + ICON_SIZE + SPACING, startY, ICON_SIZE, ICON_SIZE };

    if (mouseX >= freezeIconRect.x && mouseX <= freezeIconRect.x + freezeIconRect.w &&
        mouseY >= freezeIconRect.y && mouseY <= freezeIconRect.y + freezeIconRect.h)
    {
        if (score >= freezeButton.cost) {
            score -= freezeButton.cost;
            std::cerr << "Deducted " << freezeButton.cost << " points for freeze booster. New score: " << score << std::endl;
            currentBoosterType = BOOSTER_FREEZE;
            useFreezeBoosterOnTile();
            std::cerr << "Freeze booster activated." << std::endl;
        } else {
            std::cerr << "Not enough score for freeze booster. Current score: " << score << std::endl;
        }
    }
}

void handleTsunamiBoosterClick(int mouseX, int mouseY, SDL_Renderer* renderer) {
    const int ICON_SIZE = 50;
    const int SPACING = 20;
    int startX = GAME_AREA_WIDTH + SIDEBAR_WIDTH / 4;
    int startY = WINDOW_HEIGHT - 6 * 20;
    SDL_Rect tsunamiIconRect = { startX + 2 * (ICON_SIZE + SPACING), startY, ICON_SIZE, ICON_SIZE };

    if (mouseX >= tsunamiIconRect.x && mouseX <= tsunamiIconRect.x + tsunamiIconRect.w &&
        mouseY >= tsunamiIconRect.y && mouseY <= tsunamiIconRect.y + tsunamiIconRect.h)
    {
        if (score >= tsunamiButton.cost) {
            score -= tsunamiButton.cost;
            tsunamiActive = true;
            currentBoosterType = BOOSTER_TSUNAMI;
            std::cerr << "Tsunami booster activated." << std::endl;
        } else {
            std::cerr << "Not enough score for tsunami booster." << std::endl;
        }
    }
}

void useHammerBoosterOnTile(int mouseX, int mouseY)
{
    if (mouseX < 0 || mouseX >= GAME_AREA_WIDTH || mouseY < 0 || mouseY >= GAME_AREA_WIDTH) {
        std::cerr << "Click is outside the grid area." << std::endl;
        return;
    }

    int col = mouseX / TILE_SIZE;
    int row = mouseY / TILE_SIZE;
    std::cerr << "Hammer booster clicked tile at grid cell (" << row << ", " << col << ")." << std::endl;

    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
        std::cerr << "Calculated cell indices are out of bounds." << std::endl;
        return;
    }

    if (grid[row][col] != 0 ) {
        std::cerr << "Removing tile with value " << grid[row][col]
                  << " at (" << row << ", " << col << ")." << std::endl;
        grid[row][col] = 0;
        Mix_PlayChannel(-1, hammerSound, 0);
    } else {
        std::cerr << "No removable tile found at (" << row << ", " << col << ")." << std::endl;
    }

    if (freezeActive){
        currentBoosterType = BOOSTER_FREEZE;
    }
    hammerActive = false;
    SDL_SetCursor(SDL_GetDefaultCursor());
}

void useFreezeBoosterOnTile() {
    if (!freezeActive) {
        freezeStartTime = SDL_GetTicks();
        freezeActive = true;
        drawFreezeBoosterDuration(renderer, boosterFont);
        Mix_PlayChannel(-1, freezeSound, 0);
        std::cerr << "Freeze booster activated: Blockers will be disabled for 30 seconds." << std::endl;
    }
}

void useTsunamiBoosterOnTile(SDL_Renderer* renderer) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 0;
        }
    }
    Mix_PlayChannel(-1, tsunamiSound, 0);
    add_random_tile();
    if (freezeActive){
        currentBoosterType = BOOSTER_FREEZE;
    }
    tsunamiActive = false;
}

void drawFreezeBoosterDuration(SDL_Renderer* renderer, TTF_Font* font)
{
    if (freezeActive) {
        Uint32 elapsed = SDL_GetTicks() - freezeStartTime;
        if (elapsed < FREEZE_DURATION) {
            Uint32 remaining = FREEZE_DURATION - elapsed;
            float percentage = remaining / (float)FREEZE_DURATION;
            const int ICON_SIZE = 50;
            const int SPACING = 20;
            int startX = GAME_AREA_WIDTH + SIDEBAR_WIDTH / 4;
            int startY = WINDOW_HEIGHT - 6 * SPACING;
            SDL_Rect freezeRect = { startX + ICON_SIZE + SPACING, startY, ICON_SIZE, ICON_SIZE };
            int barHeight = 8;
            int margin = 2;
            SDL_Rect barRect = { freezeRect.x, freezeRect.y - barHeight - margin, freezeRect.w, barHeight };
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderFillRect(renderer, &barRect);
            SDL_Rect filledRect = { barRect.x, barRect.y, (int)(barRect.w * percentage), barRect.h };
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderFillRect(renderer, &filledRect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &barRect);
            double secondsRemaining = remaining / 1000.0;
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << secondsRemaining << "s";
            std::string timeStr = oss.str();
            SDL_Color textColor = {0, 0, 0, 255};
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, timeStr.c_str(), textColor);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_Rect textRect = { barRect.x + barRect.w + 5, barRect.y - 10, textSurface->w, textSurface->h };
                SDL_FreeSurface(textSurface);
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
        } else {
            freezeActive = false;
        }
    }
}
