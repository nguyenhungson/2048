#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>

const int GRID_SIZE = 4;
const int DEFAULT_VOLUME = 100;
const int DEFAULT_SFX_VOLUME = 100;
const int DEFAULT_CLOUD_BTN_WIDTH = 220;
const int DEFAULT_CLOUD_BTN_HEIGHT = 70;

// Global layout variables
int GAME_AREA_WIDTH;
int SIDEBAR_WIDTH;
int WINDOW_WIDTH;
int WINDOW_HEIGHT;
int TILE_SIZE;

// Global game state
int grid[GRID_SIZE][GRID_SIZE] = {0};
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
std::map<int, SDL_Texture*> fruitTextures;

int musicVolume = DEFAULT_VOLUME;
int sfxVolume = DEFAULT_SFX_VOLUME;

// Global game data
int incrementscore = 0;
int score = 0;
int highscore = 0;
int helpScrollOffset = 0;

// Music and Sound
Mix_Music* bgMusic = nullptr;
Mix_Music* congratsMusic = nullptr;
Mix_Music* gameWinMusic = nullptr;
Mix_Chunk* swipeSound = nullptr;
Mix_Chunk* gameOverSound = nullptr;

// Textures and Surfaces
SDL_Texture* gridBackground = nullptr;
SDL_Texture* optionBackground = nullptr;
SDL_Texture* startBackground = nullptr;
SDL_Texture* cloudTexture = nullptr;
SDL_Texture* musicbarTexture = nullptr;
SDL_Texture* musictoggleTexture = nullptr;

SDL_Surface* startSurface = nullptr;
SDL_Surface* cloudSurface = nullptr;
SDL_Surface* musicbarSurface = nullptr;
SDL_Surface* musictoggleSurface = nullptr;

// Forward declarations for functions
void loadHighscore();
void saveHighscore();
void verifyFiles();
void MusicFinishedCallback();
void loadTextures(SDL_Renderer* renderer);
void recomputeLayout(SDL_Window* window);
void drawCloudButtonWithText(SDL_Renderer* renderer, SDL_Texture* cloudTex, const SDL_Rect &btnRect, const char* text, TTF_Font* font);
void draw_start_screen(SDL_Renderer* renderer);
void draw_game_over_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont);
void draw_win_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont);
void draw_sidebar(SDL_Renderer* renderer, TTF_Font* font);
void draw_grid(SDL_Renderer* renderer, TTF_Font* font);
void draw_help_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont);
void draw_credits_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont, TTF_Font* buttonFont);
void draw_options_screen(SDL_Renderer* renderer, TTF_Font* buttonFont, TTF_Font* titleFont);
void add_random_tile();
void initialize_grid();
bool is_game_over();
bool is_game_won();
void move_tiles(SDL_Keycode key);

void loadHighscore() {
    std::ifstream infile("highscore.txt");
    if (infile.is_open()) {
        infile >> highscore;
        infile.close();
    } else {
        highscore = 0;
    }
}

void saveHighscore() {
    std::ofstream outfile("highscore.txt");
    if (outfile.is_open()) {
        outfile << highscore;
        outfile.close();
    }
}

void verifyFiles() {
    // Assume required files exist.
}

void MusicFinishedCallback() {
    if (bgMusic) {
        Mix_PlayMusic(bgMusic, -1);
    }
}

void loadTextures(SDL_Renderer* renderer) {
    std::string fruitNames[] = {
        "apple", "banana", "dragonfruit", "grape", "mango",
        "orange", "peach", "pineapple", "pomegranate", "strawberry", "watermelon"
    };
    int values[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};

    for (int i = 0; i < 11; i++) {
        std::string path = "Fruit/" + fruitNames[i] + ".jpg";
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            std::cerr << "Failed to load image: " << path
                      << " Error: " << IMG_GetError() << std::endl;
            continue;
        }
        fruitTextures[values[i]] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void recomputeLayout(SDL_Window* window) {
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

void draw_start_screen(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    if (startBackground) {
        SDL_Rect destRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, startBackground, NULL, &destRect);
    }
    SDL_RenderPresent(renderer);
}

void draw_game_over_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
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

void draw_win_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
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

void draw_help_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
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

void draw_credits_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont, TTF_Font* buttonFont) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
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

void draw_options_screen(SDL_Renderer* renderer, TTF_Font* buttonFont, TTF_Font* titleFont) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
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

void add_random_tile() {
    int empty_cells = 0;
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            if (grid[i][j] == 0)
                empty_cells++;
    if (empty_cells == 0)
        return;
    int target = rand() % empty_cells;
    int value = (rand() % 10 == 0) ? 4 : 2;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0 && target-- == 0) {
                grid[i][j] = value;
                return;
            }
        }
    }
}

void initialize_grid() {
    srand(time(0));
    score = 0;
    newHighscoreAchieved = false;
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            grid[i][j] = 0;
    for (int i = 0; i < 2; i++)
        add_random_tile();
}

bool is_game_over() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0)
                return false;
            if (grid[i][j] != 2048) {
                if (i > 0 && grid[i][j] == grid[i - 1][j])
                    return false;
                if (i < GRID_SIZE - 1 && grid[i][j] == grid[i + 1][j])
                    return false;
                if (j > 0 && grid[i][j] == grid[i][j - 1])
                    return false;
                if (j < GRID_SIZE - 1 && grid[i][j] == grid[i][j + 1])
                    return false;
            }
        }
    }
    return true;
}

bool is_game_won() {
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            if (grid[i][j] == 2048)
                return true;
    return false;
}

void move_tiles(SDL_Keycode key) {
    incrementscore = 0;
    bool moved = false;
    switch (key) {
        case SDLK_UP:
            for (int j = 0; j < GRID_SIZE; j++) {
                for (int i = 1; i < GRID_SIZE; i++) {
                    if (grid[i][j] != 0) {
                        int k = i;
                        while (k > 0 && grid[k - 1][j] == 0) {
                            grid[k - 1][j] = grid[k][j];
                            grid[k][j] = 0;
                            k--;
                            moved = true;
                        }
                        if (k > 0 && grid[k - 1][j] == grid[k][j]) {
                            if (grid[k][j] == 2048 && lock2048) {
                            } else {
                                grid[k - 1][j] *= 2;
                                incrementscore += grid[k - 1][j];
                                score += grid[k - 1][j];
                                grid[k][j] = 0;
                                moved = true;
                            }
                        }
                    }
                }
            }
            break;
        case SDLK_DOWN:
            for (int j = 0; j < GRID_SIZE; j++) {
                for (int i = GRID_SIZE - 2; i >= 0; i--) {
                    if (grid[i][j] != 0) {
                        int k = i;
                        while (k < GRID_SIZE - 1 && grid[k + 1][j] == 0) {
                            grid[k + 1][j] = grid[k][j];
                            grid[k][j] = 0;
                            k++;
                            moved = true;
                        }
                        if (k < GRID_SIZE - 1 && grid[k + 1][j] == grid[k][j]) {
                            if (grid[k][j] == 2048 && lock2048) {
                            } else {
                                grid[k + 1][j] *= 2;
                                incrementscore += grid[k + 1][j];
                                score += grid[k + 1][j];
                                grid[k][j] = 0;
                                moved = true;
                            }
                        }
                    }
                }
            }
            break;
        case SDLK_LEFT:
            for (int i = 0; i < GRID_SIZE; i++) {
                for (int j = 1; j < GRID_SIZE; j++) {
                    if (grid[i][j] != 0) {
                        int k = j;
                        while (k > 0 && grid[i][k - 1] == 0) {
                            grid[i][k - 1] = grid[i][k];
                            grid[i][k] = 0;
                            k--;
                            moved = true;
                        }
                        if (k > 0 && grid[i][k - 1] == grid[i][k]) {
                            if (grid[i][k] == 2048 && lock2048) {
                            } else {
                                grid[i][k - 1] *= 2;
                                incrementscore += grid[i][k - 1];
                                score += grid[i][k - 1];
                                grid[i][k] = 0;
                                moved = true;
                            }
                        }
                    }
                }
            }
            break;
        case SDLK_RIGHT:
            for (int i = 0; i < GRID_SIZE; i++) {
                for (int j = GRID_SIZE - 2; j >= 0; j--) {
                    if (grid[i][j] != 0) {
                        int k = j;
                        while (k < GRID_SIZE - 1 && grid[i][k + 1] == 0) {
                            grid[i][k + 1] = grid[i][k];
                            grid[i][k] = 0;
                            k++;
                            moved = true;
                        }
                        if (k < GRID_SIZE - 1 && grid[i][k + 1] == grid[i][k]) {
                            if (grid[i][k] == 2048 && lock2048) {
                            } else {
                                grid[i][k + 1] *= 2;
                                incrementscore += grid[i][k + 1];
                                score += grid[i][k + 1];
                                grid[i][k] = 0;
                                moved = true;
                            }
                        }
                    }
                }
            }
            break;
    }
    if (moved) {
        Mix_PlayChannel(-1, swipeSound, 0);
        add_random_tile();
        if (score > highscore) {
            highscore = score;
            saveHighscore();
            if (!newHighscoreAchieved) {
                newHighscoreAchieved = true;
                if (congratsMusic) {
                    Mix_HookMusicFinished(MusicFinishedCallback);
                    Mix_PlayMusic(congratsMusic, 1);
                }
            }
        }
        if (is_game_won() && !lock2048) {
            if (!gameWon) {
                gameWon = true;
                Mix_PlayMusic(gameWinMusic, -1);
            }
        }
        if (is_game_over()) {
            if (!gameOver) {
                Mix_HookMusicFinished(NULL);
                Mix_HaltMusic();
                gameOver = true;
                Mix_PlayChannel(-1, gameOverSound, 0);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL subsystems.
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << "\n";
        return 1;
    }
    if (TTF_Init() != 0) {
        std::cerr << "TTF init failed: " << TTF_GetError() << "\n";
        SDL_Quit();
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) || !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image init failed: " << IMG_GetError() << "\n";
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << "\n";
        TTF_Quit();
        SDL_Quit();
        IMG_Quit();
        return 1;
    }

    // Load music and sound effects.
    bgMusic = Mix_LoadMUS("music and sfx/linga guli guli.mp3");
    if (!bgMusic)
        std::cerr << "Failed to load background music: " << Mix_GetError() << "\n";
    else {
        Mix_VolumeMusic(musicVolume);
        Mix_PlayMusic(bgMusic, -1);
    }
    gameWinMusic = Mix_LoadMUS("music and sfx/congratulation.mp3");
    if (!gameWinMusic)
        std::cerr << "Failed to load game win music: " << Mix_GetError() << "\n";
    congratsMusic = Mix_LoadMUS("music and sfx/newrec.mp3");
    if (!congratsMusic)
        std::cerr << "Failed to load congratulation music: " << Mix_GetError() << "\n";
    swipeSound = Mix_LoadWAV("music and sfx/switch.wav");
    if (!swipeSound)
        std::cerr << "Failed to load swipe sound effect: " << Mix_GetError() << "\n";
    else {
        Mix_VolumeChunk(swipeSound, sfxVolume);
    }
    gameOverSound = Mix_LoadWAV("music and sfx/gameover.wav");
    if (!gameOverSound)
        std::cerr << "Failed to load game over sound effect: " << Mix_GetError() << "\n";
    else {
        Mix_VolumeChunk(gameOverSound, sfxVolume);
    }

    // Create window and renderer.
    SDL_Window* window = SDL_CreateWindow("2048 Fruits",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          1100, 700, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        Mix_FreeMusic(bgMusic);
        Mix_FreeChunk(swipeSound);
        Mix_CloseAudio();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        Mix_FreeMusic(bgMusic);
        Mix_FreeChunk(swipeSound);
        Mix_CloseAudio();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load PNG backgrounds and cloud image.
    startSurface = IMG_Load("backgrounds and textures/startbg.png");
    if (!startSurface)
        std::cerr << "Failed to load start PNG file: " << IMG_GetError() << "\n";

    cloudSurface = IMG_Load("backgrounds and textures/cloud.png");
    if (!cloudSurface)
        std::cerr << "Failed to load cloud PNG file: " << IMG_GetError() << "\n";

    musicbarSurface = IMG_Load("backgrounds and textures/musicbar.png");
    if (!musicbarSurface)
        std::cerr << "Failed to load music bar PNG file: " << IMG_GetError() << "\n";

    musictoggleSurface = IMG_Load("backgrounds and textures/appletoggle.png");
    if (!musictoggleSurface)
        std::cerr << "Failed to load music toggle PNG file: " << IMG_GetError() << "\n";

    gridBackground = IMG_LoadTexture(renderer, "backgrounds and textures/gamegridbg.jpg");
    if (!gridBackground)
        std::cerr << "Failed to load grid background: " << IMG_GetError() << "\n";

    optionBackground = IMG_LoadTexture(renderer, "backgrounds and textures/optionsbg.jpg");
    if (!optionBackground)
        std::cerr << "Failed to load option background: " << IMG_GetError() << "\n";

    startBackground = SDL_CreateTextureFromSurface(renderer, startSurface);
    SDL_FreeSurface(startSurface);
    if (!startBackground)
        std::cerr << "Failed to create start background texture: " << SDL_GetError() << "\n";

    cloudTexture = SDL_CreateTextureFromSurface(renderer, cloudSurface);
    SDL_FreeSurface(cloudSurface);
    if (!cloudTexture)
        std::cerr << "Failed to create cloud texture: " << SDL_GetError() << "\n";

    musicbarTexture = SDL_CreateTextureFromSurface(renderer, musicbarSurface);
    SDL_FreeSurface(musicbarSurface);
    if (!musicbarTexture)
        std::cerr << "Failed to create music bar texture: " << SDL_GetError() << "\n";

    musictoggleTexture = SDL_CreateTextureFromSurface(renderer, musictoggleSurface);
    SDL_FreeSurface(musictoggleSurface);
    if (!musictoggleTexture)
        std::cerr << "Failed to create music toggle texture: " << SDL_GetError() << "\n";

    recomputeLayout(window);
    loadTextures(renderer);
    TTF_Font* titleFont = TTF_OpenFont("font/doodle.ttf", 72);
    TTF_Font* smallFont = TTF_OpenFont("font/doodle.ttf", 42);
    TTF_Font* buttonFont = TTF_OpenFont("font/doodle.ttf", 36);
    if (!titleFont || !smallFont || !buttonFont)
        std::cerr << "Font loading error: " << TTF_GetError() << "\n";
    loadHighscore();

    // Main event loop.
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                recomputeLayout(window);
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_f) {
                    if (!isFullscreen) {
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        isFullscreen = true;
                    } else {
                        SDL_SetWindowFullscreen(window, 0);
                        isFullscreen = false;
                        recomputeLayout(window);
                    }
                    continue;
                }
                else if (e.key.keysym.sym == SDLK_m) {
                    SDL_MinimizeWindow(window);
                    continue;
                }
                if (!showOptions && !showHelp && !showCredits && !gameOver && !gameWon) {
                    if (!gameStarted) {
                        gameStarted = true;
                        gameOver = false;
                        initialize_grid();
                    } else {
                        move_tiles(e.key.keysym.sym);
                        if (is_game_over())
                            gameOver = true;
                    }
                } else {
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        showHelp = false;
                        showCredits = false;
                        showOptions = false;
                    }
                    if ((showHelp || showOptions || showCredits) &&
                        (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN)) {
                        if (e.key.keysym.sym == SDLK_UP)
                            helpScrollOffset -= 20;
                        else
                            helpScrollOffset += 20;
                        continue;
                    }
                }
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;
                if (!showOptions && !showHelp && !showCredits && !gameOver && !gameWon) {
                    SDL_Rect optionButtonRect = { GAME_AREA_WIDTH + 10, WINDOW_HEIGHT - 60, SIDEBAR_WIDTH - 20, 50 };
                    if (mouseX >= optionButtonRect.x && mouseX <= optionButtonRect.x + optionButtonRect.w &&
                        mouseY >= optionButtonRect.y && mouseY <= optionButtonRect.y + optionButtonRect.h) {
                        showOptions = true;
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
                        musicSliderActive = true;
                        continue;
                    } else if (mouseX >= sfxSliderRect.x && mouseX <= sfxSliderRect.x + sliderWidth &&
                               mouseY >= sfxSliderRect.y && mouseY <= sfxSliderRect.y + sliderHeight) {
                        sfxSliderActive = true;
                        continue;
                    }
                    const int btnWidth = 220, btnHeight2 = 70, spacing2 = 20;
                    int startY2 = 120;
                    SDL_Rect helpBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY2, btnWidth, btnHeight2 };
                    SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY2 + (btnHeight2 + spacing2), btnWidth, btnHeight2 };
                    SDL_Rect creditBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY2 + 2 * (btnHeight2 + spacing2), btnWidth, btnHeight2 };
                    SDL_Rect optionQuitBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY2 + 3 * (btnHeight2 + spacing2), btnWidth, btnHeight2 };
                    SDL_Rect backBtn = { WINDOW_WIDTH - btnWidth - 20, WINDOW_HEIGHT - btnHeight2 - 20, btnWidth, btnHeight2 };

                    if (mouseX >= helpBtn.x && mouseX <= helpBtn.x + helpBtn.w &&
                        mouseY >= helpBtn.y && mouseY <= helpBtn.y + helpBtn.h) {
                        showHelp = true;
                        showOptions = false;
                        continue;
                    } else if (mouseX >= restartBtn.x && mouseX <= restartBtn.x + restartBtn.w &&
                               mouseY >= restartBtn.y && mouseY <= restartBtn.y + restartBtn.h) {
                        initialize_grid();
                        incrementscore = 0;
                        gameStarted = true;
                        gameOver = false;
                        lock2048 = false;
                        gameWon = false;
                        showOptions = false;
                        continue;
                    } else if (mouseX >= creditBtn.x && mouseX <= creditBtn.x + creditBtn.w &&
                               mouseY >= creditBtn.y && mouseY <= creditBtn.y + creditBtn.h) {
                        showCredits = true;
                        showOptions = false;
                        continue;
                    } else if (mouseX >= optionQuitBtn.x && mouseX <= optionQuitBtn.x + optionQuitBtn.w &&
                               mouseY >= optionQuitBtn.y && mouseY <= optionQuitBtn.y + optionQuitBtn.h) {
                        quit = true;
                        continue;
                    } else if (mouseX >= backBtn.x && mouseX <= backBtn.x + backBtn.w &&
                               mouseY >= backBtn.y && mouseY <= backBtn.y + backBtn.h) {
                        showOptions = false;
                        continue;
                    }
                }
                else if (showHelp) {
                    SDL_Rect closeButtonRect = { WINDOW_WIDTH - DEFAULT_CLOUD_BTN_WIDTH - 20, WINDOW_HEIGHT - DEFAULT_CLOUD_BTN_HEIGHT - 20, DEFAULT_CLOUD_BTN_WIDTH, DEFAULT_CLOUD_BTN_HEIGHT };
                    if (mouseX >= closeButtonRect.x && mouseX <= closeButtonRect.x + closeButtonRect.w &&
                        mouseY >= closeButtonRect.y && mouseY <= closeButtonRect.y + closeButtonRect.h) {
                        showHelp = false;
                        showOptions = true;
                        continue;
                    }
                }
                else if (showCredits) {
                    SDL_Rect backButtonRect = { WINDOW_WIDTH - DEFAULT_CLOUD_BTN_WIDTH - 20, WINDOW_HEIGHT - DEFAULT_CLOUD_BTN_HEIGHT - 20, DEFAULT_CLOUD_BTN_WIDTH, DEFAULT_CLOUD_BTN_HEIGHT };
                    if (mouseX >= backButtonRect.x && mouseX <= backButtonRect.x + backButtonRect.w &&
                        mouseY >= backButtonRect.y && mouseY <= backButtonRect.y + backButtonRect.h) {
                        showCredits = false;
                        showOptions = true;
                        continue;
                    }
                }
                else if (gameOver) {
                    incrementscore = 0;
                    const int btnWidth = 200, btnHeight2 = 50, spacing2 = 20;
                    int btnStartY = (WINDOW_HEIGHT / 4) + 100;
                    SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight2 };
                    SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight2 + spacing2, btnWidth, btnHeight2 };
                    if (mouseX >= restartBtn.x && mouseX <= restartBtn.x + restartBtn.w &&
                        mouseY >= restartBtn.y && mouseY <= restartBtn.y + restartBtn.h) {
                        if (bgMusic) Mix_PlayMusic(bgMusic, -1);
                        initialize_grid();
                        gameStarted = true;
                        gameOver = false;
                        lock2048 = false;
                        continue;
                    } else if (mouseX >= quitBtn.x && mouseX <= quitBtn.x + quitBtn.w &&
                               mouseY >= quitBtn.y && mouseY <= quitBtn.y + quitBtn.h) {
                        quit = true;
                        continue;
                    }
                }
                else if (gameWon) {
                    const int btnWidth = 200, btnHeight2 = 50, spacing2 = 20;
                    int btnStartY = (WINDOW_HEIGHT / 4) + 100;
                    SDL_Rect continueBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight2 };
                    SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight2 + spacing2, btnWidth, btnHeight2 };
                    SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + 2 * (btnHeight2 + spacing2), btnWidth, btnHeight2 };
                    if (mouseX >= continueBtn.x && mouseX <= continueBtn.x + continueBtn.w &&
                        mouseY >= continueBtn.y && mouseY <= continueBtn.y + continueBtn.h) {
                        lock2048 = true;
                        Mix_HaltMusic();
                        if (bgMusic) Mix_PlayMusic(bgMusic, -1);
                        gameWon = false;
                        continue;
                    } else if (mouseX >= restartBtn.x && mouseX <= restartBtn.x + restartBtn.w &&
                               mouseY >= restartBtn.y && mouseY <= restartBtn.y + restartBtn.h) {
                        initialize_grid();
                        gameStarted = true;
                        gameOver = false;
                        lock2048 = false;
                        gameWon = false;
                        Mix_HaltMusic();
                        if (bgMusic) Mix_PlayMusic(bgMusic, -1);
                        continue;
                    } else if (mouseX >= quitBtn.x && mouseX <= quitBtn.x + quitBtn.w &&
                               mouseY >= quitBtn.y && mouseY <= quitBtn.y + quitBtn.h) {
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
    }

    for (auto& pair : fruitTextures) {
        SDL_DestroyTexture(pair.second);
    }
    TTF_CloseFont(titleFont);
    TTF_CloseFont(smallFont);
    TTF_CloseFont(buttonFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(gridBackground);
    SDL_DestroyTexture(optionBackground);
    SDL_DestroyTexture(startBackground);
    SDL_DestroyTexture(cloudTexture);
    SDL_DestroyTexture(musicbarTexture);
    SDL_DestroyTexture(musictoggleTexture);
    Mix_FreeChunk(swipeSound);
    Mix_FreeChunk(gameOverSound);
    Mix_FreeMusic(congratsMusic);
    Mix_FreeMusic(gameWinMusic);
    Mix_FreeMusic(bgMusic);
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

