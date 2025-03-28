// globals.cpp
#include "globals.h"

int GRID_SIZE = 0;
int GAME_AREA_WIDTH = 0;
int SIDEBAR_WIDTH = 0;
int WINDOW_WIDTH = 0;
int WINDOW_HEIGHT = 0;
int TILE_SIZE = 0;

int grid[4][4] = {0};
bool gameStarted = false;
bool gameOver = false;
bool gameWon = false;
bool lock2048 = false;
bool showHelp = false;
bool showOptions = false;
bool showCredits = false;
bool newHighscoreAchieved = false;
bool isFullscreen = false;
bool musicSliderActive = false;
bool sfxSliderActive = false;
bool quit = false;

const int DEFAULT_VOLUME = 100;
const int DEFAULT_SFX_VOLUME = 100;
int musicVolume = DEFAULT_VOLUME;
int sfxVolume = DEFAULT_SFX_VOLUME;

int incrementscore = 0;
int score = 0;
int highscore = 0;
int helpScrollOffset = 0;

Mix_Music* bgMusic = nullptr;
Mix_Music* congratsMusic = nullptr;
Mix_Music* gameWinMusic = nullptr;

Mix_Chunk* swipeSound = nullptr;
Mix_Chunk* gameOverSound = nullptr;

SDL_Texture* gridBackground = nullptr;
SDL_Texture* optionBackground = nullptr;
SDL_Texture* startBackground = nullptr;
SDL_Texture* cloudTexture = nullptr;
SDL_Texture* musicbarTexture = nullptr;
SDL_Texture* musictoggleTexture = nullptr;

const int DEFAULT_CLOUD_BTN_WIDTH = 150;
const int DEFAULT_CLOUD_BTN_HEIGHT = 80;
