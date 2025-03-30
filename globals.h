// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <map>

// Layout variables.
extern int GRID_SIZE;
extern int GAME_AREA_WIDTH;
extern int SIDEBAR_WIDTH;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern int TILE_SIZE;

extern SDL_Window* window;
extern SDL_Renderer* renderer;

// Game state.
extern int grid[4][4];
extern bool gameStarted;
extern bool gameOver;
extern bool gameWon;
extern bool lock2048;
extern bool showHelp;
extern bool showOptions;
extern bool showCredits;
extern bool newHighscoreAchieved;
extern bool isFullscreen;
extern bool musicSliderActive;
extern bool sfxSliderActive;
extern bool quit;
extern bool congratsShown;

// Booster
extern bool boosterActive;
extern const Uint32 BOOSTER_DURATION;
extern Uint32 boosterStartTime;
extern std::map<int, bool> boosterActivated;
struct Booster{
    int multiplier;
    Uint32 duration;
};
extern Booster currentBooster;
extern std::map<int, Booster> boosterSettings;

// Blocker
const int BLOCKER_VALUE = -1;

// Time
extern Uint32 newHighscoreTime;

// Volume settings.
extern const int DEFAULT_VOLUME;
extern const int DEFAULT_SFX_VOLUME;
extern int musicVolume;
extern int sfxVolume;

// Scores.
extern int incrementscore;
extern int score;
extern int highscore;
extern int helpScrollOffset;
extern int emptyCells;

// Index
extern int currentGameoverIndex;
extern int currentWinIndex;

// Music.
extern Mix_Music* bgMusic;
extern Mix_Music* congratsMusic;
extern Mix_Music* gameWinMusic;

// Sound effects.
extern Mix_Chunk* swipeSound;
extern Mix_Chunk* gameOverSound;

extern SDL_Texture* gridBackground;
extern SDL_Texture* optionBackground;
extern SDL_Texture* startBackground;
extern SDL_Texture* sidebarBackground;
extern SDL_Texture* scoreBackground;
extern SDL_Texture* recordBackground;
extern SDL_Texture* gameoverBackground;
extern SDL_Texture* gamewonBackground;
extern SDL_Texture* blockerTexture;
extern SDL_Texture* cloudTexture;
extern SDL_Texture* musicbarTexture;
extern SDL_Texture* musictoggleTexture;
extern SDL_Texture* congratulationTexture;

extern const int DEFAULT_CLOUD_BTN_WIDTH;
extern const int DEFAULT_CLOUD_BTN_HEIGHT;

extern TTF_Font* titleFont;
extern TTF_Font* smallFont;
extern TTF_Font* buttonFont;
extern TTF_Font* boosterFont;
extern TTF_Font* valueFont;

extern std::map<int, SDL_Texture*> fruitTextures;
extern std::map<int, SDL_Texture*> gamewinTextures;
extern std::map<int, SDL_Texture*> gameoverTextures;

#endif // GLOBALS_H

