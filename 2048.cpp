#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <map>
#include <fstream>

const int GRID_SIZE = 4;

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
bool gameWon = false;      // When a 2048 tile is created
bool lock2048 = false;     // When true, 2048 tiles cannot merge further after winning
bool showHelp = false;
bool showOptions = false;
bool showCredits = false;
std::map<int, SDL_Texture*> fruitTextures;

// Volume settings (range 0–100)
const int DEFAULT_VOLUME = 100;
const int DEFAULT_SFX_VOLUME = 100;
int musicVolume = DEFAULT_VOLUME;
int sfxVolume = DEFAULT_SFX_VOLUME;

int score = 0;
int highscore = 0;
int helpScrollOffset = 0;
bool isFullscreen = false;

// Sound effect (swipe sound)
Mix_Chunk* swipeSound = nullptr;
<<<<<<< Updated upstream
=======
Mix_Chunk* gameOverSound = nullptr;

// Game backgrounds
SDL_Texture* gridBackground = nullptr;
SDL_Texture* optionBackground = nullptr;
SDL_Texture* startBackground = nullptr;

// Game surfaces
SDL_Surface* startSurface = nullptr;
>>>>>>> Stashed changes

// Forward declarations for functions
void loadHighscore();
void saveHighscore();
void verifyFiles();
void loadTextures(SDL_Renderer* renderer);
void recomputeLayout(SDL_Window* window);
void draw_start_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont);
void draw_game_over_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont);
void draw_win_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont);
void draw_sidebar(SDL_Renderer* renderer, TTF_Font* font);
void draw_grid(SDL_Renderer* renderer, TTF_Font* font);
void draw_help_screen(SDL_Renderer* renderer, TTF_Font* font);
void draw_credits_screen(SDL_Renderer* renderer, TTF_Font* font);
void draw_options_screen(SDL_Renderer* renderer, TTF_Font* font);
void add_random_tile();
void initialize_grid();
bool is_game_over();
bool is_game_won();
void move_tiles(SDL_Keycode key);

// Function definitions

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

    // Use a 7:1 ratio (grid:sidebar) => total 8 parts.
    int proposedGameWidth = (actualW * 7) / 8;
    if (proposedGameWidth > actualH) {
        proposedGameWidth = actualH;
    }
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

void draw_start_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    if (startBackground) {
        SDL_Rect destRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
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

    int btnWidth = 200, btnHeight = 50, spacing = 20;
    int btnStartY = resultRect.y + resultRect.h + 30;
    SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight };
    SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight + spacing, btnWidth, btnHeight };

    SDL_SetRenderDrawColor(renderer, 100, 250, 100, 255);
    SDL_RenderFillRect(renderer, &restartBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &restartBtn);
    SDL_Surface* restartTextSurf = TTF_RenderText_Solid(smallFont, "Restart", textColor);
    if (restartTextSurf) {
        SDL_Texture* restartTextTex = SDL_CreateTextureFromSurface(renderer, restartTextSurf);
        SDL_Rect restartTextRect = {
            restartBtn.x + (btnWidth - restartTextSurf->w) / 2,
            restartBtn.y + (btnHeight - restartTextSurf->h) / 2,
            restartTextSurf->w,
            restartTextSurf->h
        };
        SDL_FreeSurface(restartTextSurf);
        SDL_RenderCopy(renderer, restartTextTex, NULL, &restartTextRect);
        SDL_DestroyTexture(restartTextTex);
    }

    SDL_SetRenderDrawColor(renderer, 250, 100, 100, 255);
    SDL_RenderFillRect(renderer, &quitBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &quitBtn);
    SDL_Surface* quitTextSurf = TTF_RenderText_Solid(smallFont, "Quit", textColor);
    if (quitTextSurf) {
        SDL_Texture* quitTextTex = SDL_CreateTextureFromSurface(renderer, quitTextSurf);
        SDL_Rect quitTextRect = {
            quitBtn.x + (btnWidth - quitTextSurf->w) / 2,
            quitBtn.y + (btnHeight - quitTextSurf->h) / 2,
            quitTextSurf->w,
            quitTextSurf->h
        };
        SDL_FreeSurface(quitTextSurf);
        SDL_RenderCopy(renderer, quitTextTex, NULL, &quitTextRect);
        SDL_DestroyTexture(quitTextTex);
    }

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

    int btnWidth = 200, btnHeight = 50, spacing = 20;
    int btnStartY = winLineRect.y + winLineRect.h + 30;
    SDL_Rect continueBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight };
    SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight + spacing, btnWidth, btnHeight };
    SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + 2*(btnHeight + spacing), btnWidth, btnHeight };

    SDL_SetRenderDrawColor(renderer, 100, 200, 250, 255);
    SDL_RenderFillRect(renderer, &continueBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &continueBtn);
    SDL_Surface* contTextSurf = TTF_RenderText_Solid(smallFont, "Continue", textColor);
    if (contTextSurf) {
        SDL_Texture* contTextTex = SDL_CreateTextureFromSurface(renderer, contTextSurf);
        SDL_Rect contTextRect = {
            continueBtn.x + (btnWidth - contTextSurf->w) / 2,
            continueBtn.y + (btnHeight - contTextSurf->h) / 2,
            contTextSurf->w,
            contTextSurf->h
        };
        SDL_FreeSurface(contTextSurf);
        SDL_RenderCopy(renderer, contTextTex, NULL, &contTextRect);
        SDL_DestroyTexture(contTextTex);
    }

    SDL_SetRenderDrawColor(renderer, 100, 250, 100, 255);
    SDL_RenderFillRect(renderer, &restartBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &restartBtn);
    SDL_Surface* restartTextSurf = TTF_RenderText_Solid(smallFont, "Restart", textColor);
    if (restartTextSurf) {
        SDL_Texture* restartTextTex = SDL_CreateTextureFromSurface(renderer, restartTextSurf);
        SDL_Rect restartTextRect = {
            restartBtn.x + (btnWidth - restartTextSurf->w) / 2,
            restartBtn.y + (btnHeight - restartTextSurf->h) / 2,
            restartTextSurf->w,
            restartTextSurf->h
        };
        SDL_FreeSurface(restartTextSurf);
        SDL_RenderCopy(renderer, restartTextTex, NULL, &restartTextRect);
        SDL_DestroyTexture(restartTextTex);
    }

    SDL_SetRenderDrawColor(renderer, 250, 100, 250, 255);
    SDL_RenderFillRect(renderer, &quitBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &quitBtn);
    SDL_Surface* quitTextSurf = TTF_RenderText_Solid(smallFont, "Quit", textColor);
    if (quitTextSurf) {
        SDL_Texture* quitTextTex = SDL_CreateTextureFromSurface(renderer, quitTextSurf);
        SDL_Rect quitTextRect = {
            quitBtn.x + (btnWidth - quitTextSurf->w) / 2,
            quitBtn.y + (btnHeight - quitTextSurf->h) / 2,
            quitTextSurf->w,
            quitTextSurf->h
        };
        SDL_FreeSurface(quitTextSurf);
        SDL_RenderCopy(renderer, quitTextTex, NULL, &quitTextRect);
        SDL_DestroyTexture(quitTextTex);
    }

    SDL_RenderPresent(renderer);
}

void draw_sidebar(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Rect sidebarRect = { GAME_AREA_WIDTH, 0, SIDEBAR_WIDTH, WINDOW_HEIGHT };
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderFillRect(renderer, &sidebarRect);
    SDL_Color textColor = {0, 0, 0, 255};

    std::string scoreText = "Score: " + std::to_string(score);
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

    // Draw Options button.
    SDL_Rect optionButtonRect = { GAME_AREA_WIDTH + 10, WINDOW_HEIGHT - 60, SIDEBAR_WIDTH - 20, 50 };
    SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
    SDL_RenderFillRect(renderer, &optionButtonRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &optionButtonRect);
    std::string optionText = "Options";
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* optionSurface = TTF_RenderText_Solid(font, optionText.c_str(), white);
    if (optionSurface) {
        SDL_Texture* optionTexture = SDL_CreateTextureFromSurface(renderer, optionSurface);
        SDL_Rect textRect = {
            optionButtonRect.x + (optionButtonRect.w - optionSurface->w) / 2,
            optionButtonRect.y + (optionButtonRect.h - optionSurface->h) / 2,
            optionSurface->w, optionSurface->h
        };
        SDL_FreeSurface(optionSurface);
        SDL_RenderCopy(renderer, optionTexture, NULL, &textRect);
        SDL_DestroyTexture(optionTexture);
    }
    SDL_RenderPresent(renderer);
}

void draw_grid(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int value = grid[i][j];
            SDL_Rect rect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_SetRenderDrawColor(renderer, 205, 193, 180, 255);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
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

void draw_help_screen(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    if (optionBackground) {
        SDL_Rect destRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, optionBackground, NULL, &destRect);
    }
    SDL_Color textColor = {0, 0, 0, 255};
    std::string instructions[] = {
        "                       Welcome to 2048 Fruits!",
        "",
        "* Objective:",
        "Combine matching fruit tiles by sliding them",
        "with the arrow keys. When two tiles with the",
        "same value collide, they merge into one with",
        "double the points. Reach the 2048 tile while",
        "achieving the highest score.",
        "",
        "* How to Play:",
        "- Use arrow keys to move tiles.",
        "- Identical tiles merge to double points.",
        "- Game over when no moves remain.",
        "- Highscore is saved persistently.",
        "",
        "* Fruit to Points:",
        "- Apple: 2",
        "- Banana: 4",
        "- Dragonfruit: 8",
        "- Grape: 16",
        "- Mango: 32",
        "- Orange: 64",
        "- Peach: 128",
        "- Pineapple: 256",
        "- Pomegranate: 512",
        "- Strawberry: 1024",
        "- Watermelon: 2048",
        ""
    };
    int lineHeight = TTF_FontLineSkip(font);
    int numLines = sizeof(instructions) / sizeof(instructions[0]);
    int totalHeight = numLines * lineHeight;
    if (helpScrollOffset < 0) helpScrollOffset = 0;
    if (helpScrollOffset > totalHeight - WINDOW_HEIGHT)
        helpScrollOffset = totalHeight - WINDOW_HEIGHT;
    int y = 20 - helpScrollOffset;
    for (int i = 0; i < numLines; i++) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, instructions[i].c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {20, y, textSurface->w, textSurface->h};
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
            y += lineHeight;
        }
    }
    SDL_Rect closeButtonRect = { WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50 };
    SDL_SetRenderDrawColor(renderer, 250, 100, 100, 255);
    SDL_RenderFillRect(renderer, &closeButtonRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &closeButtonRect);
    std::string closeText = "Close";
    SDL_Surface* closeSurface = TTF_RenderText_Solid(font, closeText.c_str(), {255, 255, 255, 255});
    if (closeSurface) {
        SDL_Texture* closeTexture = SDL_CreateTextureFromSurface(renderer, closeSurface);
        SDL_Rect closeTextRect = {
            closeButtonRect.x + (closeButtonRect.w - closeSurface->w) / 2,
            closeButtonRect.y + (closeButtonRect.h - closeSurface->h) / 2,
            closeSurface->w,
            closeSurface->h
        };
        SDL_FreeSurface(closeSurface);
        SDL_RenderCopy(renderer, closeTexture, NULL, &closeTextRect);
        SDL_DestroyTexture(closeTexture);
    }
    SDL_RenderPresent(renderer);
}

void draw_credits_screen(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    if (optionBackground) {
        SDL_Rect destRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, optionBackground, NULL, &destRect);
    }
    SDL_Color textColor = {0, 0, 0, 255};
<<<<<<< Updated upstream
    std::string credits = "Credits:\n\nDeveloped by Nguyen Hung Son";
=======
    std::string credits = "                                        Credits\n\n            Developed by MVPSON98 HARDSTUCK SILVER\n\n            PROPS TO DATSKII FOR THE LOVELY ARTWORK";
>>>>>>> Stashed changes
    SDL_Surface* creditSurface = TTF_RenderText_Blended_Wrapped(font, credits.c_str(), textColor, WINDOW_WIDTH - 40);
    if (creditSurface) {
        SDL_Texture* creditTexture = SDL_CreateTextureFromSurface(renderer, creditSurface);
        SDL_Rect creditRect = {20, 20, creditSurface->w, creditSurface->h};
        SDL_FreeSurface(creditSurface);
        SDL_RenderCopy(renderer, creditTexture, NULL, &creditRect);
        SDL_DestroyTexture(creditTexture);
    }
    SDL_Rect backBtn = { WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50 };
    SDL_SetRenderDrawColor(renderer, 200, 200, 100, 255);
    SDL_RenderFillRect(renderer, &backBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &backBtn);
    SDL_Surface* backTextSurf = TTF_RenderText_Solid(font, "Back", textColor);
    if (backTextSurf) {
        SDL_Texture* backTextTex = SDL_CreateTextureFromSurface(renderer, backTextSurf);
        SDL_Rect backTextRect = {
            backBtn.x + (backBtn.w - backTextSurf->w) / 2,
            backBtn.y + (backBtn.h - backTextSurf->h) / 2,
            backTextSurf->w,
            backTextSurf->h
        };
        SDL_FreeSurface(backTextSurf);
        SDL_RenderCopy(renderer, backTextTex, NULL, &backTextRect);
        SDL_DestroyTexture(backTextTex);
    }
    SDL_RenderPresent(renderer);
}

void draw_options_screen(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    if (optionBackground) {
        SDL_Rect destRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, optionBackground, NULL, &destRect);
    }
    SDL_Color textColor = {0, 0, 0, 255};

    // Title "Options"
    SDL_Surface* titleSurf = TTF_RenderText_Solid(font, "Options", textColor);
    SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurf);
    SDL_Rect titleRect = { (WINDOW_WIDTH - titleSurf->w) / 2, 50, titleSurf->w, titleSurf->h };
    SDL_FreeSurface(titleSurf);
    SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
    SDL_DestroyTexture(titleTex);

    int btnWidth = 200, btnHeight = 50, spacing = 20;
    int startY = 120;
    SDL_Rect helpBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY, btnWidth, btnHeight };
    SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY + btnHeight + spacing, btnWidth, btnHeight };
    SDL_Rect creditBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY + 2 * (btnHeight + spacing), btnWidth, btnHeight };
    SDL_Rect optionQuitBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY + 3 * (btnHeight + spacing), btnWidth, btnHeight };
    SDL_Rect backBtn = { WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50 };

    // Draw Help button.
    SDL_SetRenderDrawColor(renderer, 100, 100, 250, 255);
    SDL_RenderFillRect(renderer, &helpBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &helpBtn);
    SDL_Surface* helpTextSurf = TTF_RenderText_Solid(font, "Help", textColor);
    if (helpTextSurf) {
        SDL_Texture* helpTextTex = SDL_CreateTextureFromSurface(renderer, helpTextSurf);
        SDL_Rect helpTextRect = { helpBtn.x + (btnWidth - helpTextSurf->w) / 2,
                                  helpBtn.y + (btnHeight - helpTextSurf->h) / 2,
                                  helpTextSurf->w, helpTextSurf->h };
        SDL_FreeSurface(helpTextSurf);
        SDL_RenderCopy(renderer, helpTextTex, NULL, &helpTextRect);
        SDL_DestroyTexture(helpTextTex);
    }

    // Draw Restart button.
    SDL_SetRenderDrawColor(renderer, 100, 250, 100, 255);
    SDL_RenderFillRect(renderer, &restartBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &restartBtn);
    SDL_Surface* restartTextSurf = TTF_RenderText_Solid(font, "Restart", textColor);
    if (restartTextSurf) {
        SDL_Texture* restartTextTex = SDL_CreateTextureFromSurface(renderer, restartTextSurf);
        SDL_Rect restartTextRect = { restartBtn.x + (btnWidth - restartTextSurf->w) / 2,
                                     restartBtn.y + (btnHeight - restartTextSurf->h) / 2,
                                     restartTextSurf->w, restartTextSurf->h };
        SDL_FreeSurface(restartTextSurf);
        SDL_RenderCopy(renderer, restartTextTex, NULL, &restartTextRect);
        SDL_DestroyTexture(restartTextTex);
    }

    // Draw Credits button.
    SDL_SetRenderDrawColor(renderer, 250, 100, 100, 255);
    SDL_RenderFillRect(renderer, &creditBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &creditBtn);
    SDL_Surface* creditTextSurf = TTF_RenderText_Solid(font, "Credits", textColor);
    if (creditTextSurf) {
        SDL_Texture* creditTextTex = SDL_CreateTextureFromSurface(renderer, creditTextSurf);
        SDL_Rect creditTextRect = { creditBtn.x + (btnWidth - creditTextSurf->w) / 2,
                                    creditBtn.y + (btnHeight - creditTextSurf->h) / 2,
                                    creditTextSurf->w, creditTextSurf->h };
        SDL_FreeSurface(creditTextSurf);
        SDL_RenderCopy(renderer, creditTextTex, NULL, &creditTextRect);
        SDL_DestroyTexture(creditTextTex);
    }

    // Draw Quit button.
    SDL_SetRenderDrawColor(renderer, 250, 100, 250, 255);
    SDL_RenderFillRect(renderer, &optionQuitBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &optionQuitBtn);
    SDL_Surface* quitTextSurf = TTF_RenderText_Solid(font, "Quit", textColor);
    if (quitTextSurf) {
        SDL_Texture* quitTextTex = SDL_CreateTextureFromSurface(renderer, quitTextSurf);
        SDL_Rect quitTextRect = { optionQuitBtn.x + (btnWidth - quitTextSurf->w) / 2,
                                  optionQuitBtn.y + (btnHeight - quitTextSurf->h) / 2,
                                  quitTextSurf->w, quitTextSurf->h };
        SDL_FreeSurface(quitTextSurf);
        SDL_RenderCopy(renderer, quitTextTex, NULL, &quitTextRect);
        SDL_DestroyTexture(quitTextTex);
    }

    // Draw Music Volume Slider.
    int sliderWidth = 300, sliderHeight = 20;
    int sliderX = (WINDOW_WIDTH - sliderWidth) / 2;
    int baseY = 120, btnHeight2 = 50, spacing2 = 20;
    int musicSliderY = baseY + 3 * (btnHeight2 + spacing2) + 40;
    SDL_Rect musicSliderBg = { sliderX, musicSliderY, sliderWidth, sliderHeight };
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderFillRect(renderer, &musicSliderBg);
    int handleWidth = 10;
    int musicHandleX = sliderX + (musicVolume * (sliderWidth - handleWidth)) / DEFAULT_VOLUME;
    SDL_Rect musicHandleRect = { musicHandleX, musicSliderY - 5, handleWidth, sliderHeight + 10 };
    SDL_SetRenderDrawColor(renderer, 100, 100, 250, 255);
    SDL_RenderFillRect(renderer, &musicHandleRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &musicSliderBg);

    // Draw Music label.
    SDL_Surface* musicLabelSurf = TTF_RenderText_Solid(font, "Music", {0,0,0,255});
    if (musicLabelSurf) {
        SDL_Texture* musicLabelTex = SDL_CreateTextureFromSurface(renderer, musicLabelSurf);
        SDL_Rect musicLabelRect = { sliderX, musicSliderY - 30, musicLabelSurf->w, musicLabelSurf->h };
        SDL_FreeSurface(musicLabelSurf);
        SDL_RenderCopy(renderer, musicLabelTex, NULL, &musicLabelRect);
        SDL_DestroyTexture(musicLabelTex);
    }

    // Draw SFX Volume Slider.
    int sfxSliderY = musicSliderY + 50;
    SDL_Rect sfxSliderBg = { sliderX, sfxSliderY, sliderWidth, sliderHeight };
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderFillRect(renderer, &sfxSliderBg);
    int sfxHandleX = sliderX + (sfxVolume * (sliderWidth - handleWidth)) / DEFAULT_SFX_VOLUME;
    SDL_Rect sfxHandleRect = { sfxHandleX, sfxSliderY - 5, handleWidth, sliderHeight + 10 };
    SDL_SetRenderDrawColor(renderer, 100, 100, 250, 255);
    SDL_RenderFillRect(renderer, &sfxHandleRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &sfxSliderBg);

    // Draw SFX label.
    SDL_Surface* sfxLabelSurf = TTF_RenderText_Solid(font, "SFX", {0,0,0,255});
    if (sfxLabelSurf) {
        SDL_Texture* sfxLabelTex = SDL_CreateTextureFromSurface(renderer, sfxLabelSurf);
        SDL_Rect sfxLabelRect = { sliderX, sfxSliderY - 30, sfxLabelSurf->w, sfxLabelSurf->h };
        SDL_FreeSurface(sfxLabelSurf);
        SDL_RenderCopy(renderer, sfxLabelTex, NULL, &sfxLabelRect);
        SDL_DestroyTexture(sfxLabelTex);
    }

    // Draw Back button.
    SDL_Rect backButtonRect = { WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50 };
    SDL_SetRenderDrawColor(renderer, 200, 200, 100, 255);
    SDL_RenderFillRect(renderer, &backButtonRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &backButtonRect);
    SDL_Surface* backTextSurf = TTF_RenderText_Solid(font, "Back", textColor);
    if (backTextSurf) {
        SDL_Texture* backTextTex = SDL_CreateTextureFromSurface(renderer, backTextSurf);
        SDL_Rect backTextRect = { backButtonRect.x + (backButtonRect.w - backTextSurf->w) / 2,
                                  backButtonRect.y + (backButtonRect.h - backTextSurf->h) / 2,
                                  backTextSurf->w, backTextSurf->h };
        SDL_FreeSurface(backTextSurf);
        SDL_RenderCopy(renderer, backTextTex, NULL, &backTextRect);
        SDL_DestroyTexture(backTextTex);
    }
    SDL_RenderPresent(renderer);
}

void add_random_tile() {
    int empty_cells = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) empty_cells++;
        }
    }
    if (empty_cells == 0) return;
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
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            grid[i][j] = 0;
    for (int i = 0; i < 2; i++)
        add_random_tile();
}

bool is_game_over() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            // If there is an empty cell, the game is not over.
            if (grid[i][j] == 0)
                return false;
            // Only consider merging if the tile is not 2048.
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
                                // Do not merge 2048 if locked.
                            } else {
                                grid[k - 1][j] *= 2;
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
        add_random_tile();
        if (score > highscore) {
            highscore = score;
            saveHighscore();
        }
        if (is_game_won() && !lock2048) {
            gameWon = true;
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << "\n";
        return 1;
    }
    if (TTF_Init() != 0) {
        std::cerr << "TTF init failed: " << TTF_GetError() << "\n";
        SDL_Quit();
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
        std::cerr << "SDL_image init failed: " << IMG_GetError() << "\n";
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image init failed: " << IMG_GetError() << "\n";
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << "\n";
        TTF_Quit();
        SDL_Quit();
        IMG_Quit();
        return 1;
    }

    Mix_Music* bgMusic = Mix_LoadMUS("mouse repellent.mp3");
    if (!bgMusic) {
        std::cerr << "Failed to load background music: " << Mix_GetError() << "\n";
    } else {
        Mix_PlayMusic(bgMusic, -1);
    }

    swipeSound = Mix_LoadWAV("duckquack.wav");
    if (!swipeSound) {
        std::cerr << "Failed to load swipe sound effect: " << Mix_GetError() << "\n";
    } else {
        Mix_VolumeChunk(swipeSound, sfxVolume);
    }

    SDL_Window* window = SDL_CreateWindow(
        "2048 Fruits",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1000, 700,
        SDL_WINDOW_RESIZABLE
    );
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
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
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

<<<<<<< Updated upstream
=======
    //PNG Surface
    startSurface = IMG_Load("startbg.png");
    if (!startSurface) {
        std::cerr << "Failed to load start PNG file: " << IMG_GetError() << "\n";
    }

    //JPG textures
    gridBackground = IMG_LoadTexture(renderer, "gamegridbg.jpg");
    if (!gridBackground) {
        std::cerr << "Failed to load grid background: " << IMG_GetError() << std::endl;
    }

    optionBackground = IMG_LoadTexture(renderer, "optionsbg.jpg");
    if (!optionBackground) {
        std::cerr << "Failed to load option background: " << IMG_GetError() << std::endl;
    }

    startBackground = SDL_CreateTextureFromSurface(renderer, startSurface);
    SDL_FreeSurface(startSurface);
    if (!startBackground) {
        std::cerr << "Failed to load start background: " << SDL_GetError() << "\n";
    }

>>>>>>> Stashed changes
    recomputeLayout(window);
    loadTextures(renderer);
    TTF_Font* titleFont = TTF_OpenFont("doodle.ttf", 72);
    TTF_Font* smallFont = TTF_OpenFont("doodle.ttf", 48);
    if (!titleFont || !smallFont) {
        std::cerr << "Font loading error: " << TTF_GetError() << "\n";
    }
    loadHighscore();

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
                        if (showHelp || showCredits) {
                            showHelp = false;
                            showCredits = false;
                            showOptions = true;
                        } else {
                            showOptions = false;
                        }
                    }
                    // Handle menu scrolling with up/down arrow keys.
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
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
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
                        int sliderWidth = 300, sliderHeight = 20;
                        int sliderX = (WINDOW_WIDTH - sliderWidth) / 2;
                        int baseY = 120, btnHeight = 50, spacing = 20;
                        int musicSliderY = baseY + 3 * (btnHeight + spacing) + 40;
                        int sfxSliderY = musicSliderY + 50;
                        SDL_Rect musicSliderRect = { sliderX, musicSliderY, sliderWidth, sliderHeight };
                        SDL_Rect sfxSliderRect = { sliderX, sfxSliderY, sliderWidth, sliderHeight };
                        if (mouseX >= musicSliderRect.x && mouseX <= musicSliderRect.x + musicSliderRect.w &&
                            mouseY >= musicSliderRect.y && mouseY <= musicSliderRect.y + musicSliderRect.h) {
                            int clampedX = mouseX;
                            if (clampedX < sliderX) clampedX = sliderX;
                            if (clampedX > sliderX + sliderWidth) clampedX = sliderX + sliderWidth;
                            int sliderPosition = clampedX - sliderX;
                            musicVolume = (sliderPosition * DEFAULT_VOLUME) / sliderWidth;
                            Mix_VolumeMusic(musicVolume);
                            continue;
                        }
                        else if (mouseX >= sfxSliderRect.x && mouseX <= sfxSliderRect.x + sfxSliderRect.w &&
                                 mouseY >= sfxSliderRect.y && mouseY <= sfxSliderRect.y + sfxSliderRect.h) {
                            int clampedX = mouseX;
                            if (clampedX < sliderX) clampedX = sliderX;
                            if (clampedX > sliderX + sliderWidth) clampedX = sliderX + sliderWidth;
                            int sliderPosition = clampedX - sliderX;
                            sfxVolume = (sliderPosition * DEFAULT_SFX_VOLUME) / sliderWidth;
                            Mix_VolumeChunk(swipeSound, sfxVolume);
                            continue;
                        }
                        int btnWidth = 200, btnHeight2 = 50, spacing2 = 20;
                        int startY2 = 120;
                        SDL_Rect helpBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY2, btnWidth, btnHeight2 };
                        SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY2 + btnHeight2 + spacing2, btnWidth, btnHeight2 };
                        SDL_Rect creditBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY2 + 2 * (btnHeight2 + spacing2), btnWidth, btnHeight2 };
                        SDL_Rect optionQuitBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY2 + 3 * (btnHeight2 + spacing2), btnWidth, btnHeight2 };
                        SDL_Rect backBtn = { WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50 };
                        if (mouseX >= helpBtn.x && mouseX <= helpBtn.x + helpBtn.w &&
                            mouseY >= helpBtn.y && mouseY <= helpBtn.y + helpBtn.h) {
                            showHelp = true;
                            showOptions = false;
                            continue;
                        } else if (mouseX >= restartBtn.x && mouseX <= restartBtn.x + restartBtn.w &&
                                   mouseY >= restartBtn.y && mouseY <= restartBtn.y + restartBtn.h) {
                            initialize_grid();
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
                        SDL_Rect closeButtonRect = { WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50 };
                        if (mouseX >= closeButtonRect.x && mouseX <= closeButtonRect.x + closeButtonRect.w &&
                            mouseY >= closeButtonRect.y && mouseY <= closeButtonRect.y + closeButtonRect.h) {
                            showHelp = false;
                            showOptions = true;
                            continue;
                        }
                    }
                    else if (showCredits) {
                        SDL_Rect backButtonRect = { WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50 };
                        if (mouseX >= backButtonRect.x && mouseX <= backButtonRect.x + backButtonRect.w &&
                            mouseY >= backButtonRect.y && mouseY <= backButtonRect.y + backButtonRect.h) {
                            showCredits = false;
                            showOptions = true;
                            continue;
                        }
                    }
                    else if (gameOver) {
                        int btnWidth = 200, btnHeight2 = 50, spacing2 = 20;
                        int btnStartY = (WINDOW_HEIGHT / 4) + 100;
                        SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight2 };
                        SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight2 + spacing2, btnWidth, btnHeight2 };
                        if (mouseX >= restartBtn.x && mouseX <= restartBtn.x + restartBtn.w &&
                            mouseY >= restartBtn.y && mouseY <= restartBtn.y + restartBtn.h) {
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
                        int btnWidth = 200, btnHeight2 = 50, spacing2 = 20;
                        int btnStartY = (WINDOW_HEIGHT / 4) + 100;
                        SDL_Rect continueBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight2 };
                        SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight2 + spacing2, btnWidth, btnHeight2 };
                        SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + 2 * (btnHeight2 + spacing2), btnWidth, btnHeight2 };
                        if (mouseX >= continueBtn.x && mouseX <= continueBtn.x + continueBtn.w &&
                            mouseY >= continueBtn.y && mouseY <= continueBtn.y + continueBtn.h) {
                            lock2048 = true;  // Lock further merging of 2048 tiles.
                            gameWon = false;
                            continue;
                        } else if (mouseX >= restartBtn.x && mouseX <= restartBtn.x + restartBtn.w &&
                                   mouseY >= restartBtn.y && mouseY <= restartBtn.y + restartBtn.h) {
                            initialize_grid();
                            gameStarted = true;
                            gameOver = false;
                            lock2048 = false;
                            gameWon = false;
                            continue;
                        } else if (mouseX >= quitBtn.x && mouseX <= quitBtn.x + quitBtn.w &&
                                   mouseY >= quitBtn.y && mouseY <= quitBtn.y + quitBtn.h) {
                            quit = true;
                            continue;
                        }
                    }
                }
            }
            else if (showOptions && e.type == SDL_MOUSEMOTION && (e.motion.state & SDL_BUTTON_LMASK)) {
                int sliderWidth = 300, sliderHeight = 20;
                int sliderX = (WINDOW_WIDTH - sliderWidth) / 2;
                int baseY = 120, btnHeight = 50, spacing = 20;
                int musicSliderY = baseY + 3 * (btnHeight + spacing) + 40;
                int sfxSliderY = musicSliderY + 50;
                SDL_Rect musicSliderRect = { sliderX, musicSliderY, sliderWidth, sliderHeight };
                if (e.motion.x >= musicSliderRect.x && e.motion.x <= musicSliderRect.x + musicSliderRect.w &&
                    e.motion.y >= musicSliderRect.y && e.motion.y <= musicSliderRect.y + musicSliderRect.h) {
                    int clampedX = e.motion.x;
                    if (clampedX < sliderX) clampedX = sliderX;
                    if (clampedX > sliderX + sliderWidth) clampedX = sliderX + sliderWidth;
                    int sliderPosition = clampedX - sliderX;
                    musicVolume = (sliderPosition * DEFAULT_VOLUME) / sliderWidth;
                    Mix_VolumeMusic(musicVolume);
                }
                SDL_Rect sfxSliderRect = { sliderX, sfxSliderY, sliderWidth, sliderHeight };
                if (e.motion.x >= sfxSliderRect.x && e.motion.x <= sfxSliderRect.x + sfxSliderRect.w &&
                    e.motion.y >= sfxSliderRect.y && e.motion.y <= sfxSliderRect.y + sfxSliderRect.h) {
                    int clampedX = e.motion.x;
                    if (clampedX < sliderX) clampedX = sliderX;
                    if (clampedX > sliderX + sliderWidth) clampedX = sliderX + sliderWidth;
                    int sliderPosition = clampedX - sliderX;
                    sfxVolume = (sliderPosition * DEFAULT_SFX_VOLUME) / sliderWidth;
                    Mix_VolumeChunk(swipeSound, sfxVolume);
                }
            }
        }

        // Render appropriate screen.
        if (gameWon) {
            draw_win_screen(renderer, titleFont, smallFont);
        }
        else if (showOptions) {
            draw_options_screen(renderer, smallFont);
        }
        else if (showHelp) {
            draw_help_screen(renderer, smallFont);
        }
        else if (showCredits) {
            draw_credits_screen(renderer, smallFont);
        }
        else if (gameOver) {
            draw_game_over_screen(renderer, titleFont, smallFont);
        }
        else if (!gameStarted) {
            draw_start_screen(renderer, titleFont, smallFont);
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
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
<<<<<<< Updated upstream
=======
    SDL_DestroyTexture(gridBackground);
    SDL_DestroyTexture(optionBackground);
    SDL_DestroyTexture(startBackground);
>>>>>>> Stashed changes
    Mix_FreeChunk(swipeSound);
    Mix_FreeMusic(bgMusic);
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

