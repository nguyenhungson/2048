// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <map>

// Layout variables.
extern int GRID_SIZE;
extern int GAME_AREA_WIDTH;
extern int SIDEBAR_WIDTH;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern int TILE_SIZE;

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

// Music.
extern Mix_Music* bgMusic;
extern Mix_Music* congratsMusic;
extern Mix_Music* gameWinMusic;

// Sound effects.
extern Mix_Chunk* swipeSound;
extern Mix_Chunk* gameOverSound;

// Textures.
extern SDL_Texture* gridBackground;
extern SDL_Texture* optionBackground;
extern SDL_Texture* startBackground;
extern SDL_Texture* cloudTexture;
extern SDL_Texture* musicbarTexture;
extern SDL_Texture* musictoggleTexture;

// (If you have default sizes for cloud buttons, declare them as well)
extern const int DEFAULT_CLOUD_BTN_WIDTH;
extern const int DEFAULT_CLOUD_BTN_HEIGHT;

#endif // GLOBALS_H

