// globals.cpp
#include "globals.h"

int GRID_SIZE = 4;
int GAME_AREA_WIDTH = 0;
int SIDEBAR_WIDTH = 0;
int WINDOW_WIDTH = 0;
int WINDOW_HEIGHT = 0;
int TILE_SIZE = 0;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

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
bool congratsShown = false;

bool boosterActive = false;
Uint32 boosterStartTime = 0;
const Uint32 BOOSTER_DURATION = 10000;
std::map<int, bool> boosterActivated;
Booster currentBooster = {100, 0};
std::map<int, Booster> boosterSettings = {
    { 2, {100, 0} },
    { 4, {120, 10000} },
    { 8, {130, 10000} },
    { 16, {140, 10000} },
    { 32, {150, 10000} },
    { 64, {160, 10000} },
    { 128, {170, 10000} },
    { 256, {180, 10000} },
    { 512, {190, 10000} },
    { 1024, {200, 10000} },
    { 2048, {210, 10000} },
};

Uint32 newHighscoreTime = 0;

const int DEFAULT_VOLUME = 100;
const int DEFAULT_SFX_VOLUME = 100;
int musicVolume = DEFAULT_VOLUME;
int sfxVolume = DEFAULT_SFX_VOLUME;

int incrementscore = 0;
int score = 0;
int highscore = 0;
int helpScrollOffset = 0;
int emptyCells = 0;

int currentGameoverIndex = 0;
int currentWinIndex = 0;

Mix_Music* bgMusic = nullptr;
Mix_Music* congratsMusic = nullptr;
Mix_Music* gameWinMusic = nullptr;

Mix_Chunk* swipeSound = nullptr;
Mix_Chunk* gameOverSound = nullptr;

SDL_Texture* gridBackground = nullptr;
SDL_Texture* optionBackground = nullptr;
SDL_Texture* startBackground = nullptr;
SDL_Texture* sidebarBackground = nullptr;
SDL_Texture* scoreBackground = nullptr;
SDL_Texture* recordBackground = nullptr;
SDL_Texture* gameoverBackground = nullptr;
SDL_Texture* gamewonBackground = nullptr;
SDL_Texture* blockerTexture = nullptr;
SDL_Texture* cloudTexture = nullptr;
SDL_Texture* musicbarTexture = nullptr;
SDL_Texture* musictoggleTexture = nullptr;

const int DEFAULT_CLOUD_BTN_WIDTH = 150;
const int DEFAULT_CLOUD_BTN_HEIGHT = 80;

TTF_Font* titleFont   = nullptr;
TTF_Font* smallFont   = nullptr;
TTF_Font* buttonFont  = nullptr;
TTF_Font* boosterFont = nullptr;
TTF_Font* valueFont   = nullptr;

std::map<int, SDL_Texture*> fruitTextures;
std::map<int, SDL_Texture*> gamewinTextures;
std::map<int, SDL_Texture*> gameoverTextures;




