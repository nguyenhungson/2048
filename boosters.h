#ifndef BOOSTERS_H_INCLUDED
#define BOOSTERS_H_INCLUDED

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>

// Enumeration for different booster types.
enum BoosterType {
    BOOSTER_HAMMER,
    BOOSTER_FREEZE,
    BOOSTER_TSUNAMI
};

// Structure representing a booster button.
struct BoosterButton {
    BoosterType type;
    int cost;
    SDL_Texture* iconTexture;
    SDL_Texture* cursorTexture;
};

//Sound effect
extern Mix_Chunk* hammerSound;
extern Mix_Chunk* freezeSound;
extern Mix_Chunk* tsunamiSound;

extern bool hammerActive;
extern bool freezeActive;
extern bool tsunamiActive;
extern Uint32 freezeStartTime;
const Uint32 FREEZE_DURATION = 30000; // 30 seconds

// Extern declarations for the booster buttons.
extern BoosterButton hammerButton;
extern BoosterButton freezeButton;
extern BoosterButton tsunamiButton;
extern BoosterType currentBoosterType;

// Loads PNG textures for the booster icons. Returns true if successful.
bool loadBoosterTextures(SDL_Renderer* renderer);

// Frees all booster-related textures.
void freeBoosterTextures(SDL_Renderer* renderer);

// Draws the booster icons on the sidebar at fixed positions.
void drawBoosterIcons(SDL_Renderer* renderer);

// Sets and returns a new SDL_Cursor for the given booster button.
// It loads a specific cursor image based on the booster type.
SDL_Cursor* setBoosterCursor(SDL_Renderer* renderer, BoosterButton button);

// Handles a click on the hammer booster icon. Call this from your mouse event handling.
void handleHammerBoosterClick(int mouseX, int mouseY, SDL_Renderer* renderer);

void handleFreezeBoosterClick(int mouseX, int mouseY, SDL_Renderer* renderer);

void handleTsunamiBoosterClick(int mouseX, int mouseY, SDL_Renderer* renderer);

void useHammerBoosterOnTile(int mouseX, int mouseY);

void useFreezeBoosterOnTile();

void useTsunamiBoosterOnTile(SDL_Renderer* renderer);

void drawFreezeBoosterDuration(SDL_Renderer* renderer, TTF_Font* font);
#endif // BOOSTERS_H_INCLUDED

