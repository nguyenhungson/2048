#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <map>
#include <fstream>

const int GRID_SIZE = 4;

int GAME_AREA_WIDTH;
int SIDEBAR_WIDTH;
int WINDOW_WIDTH;
int WINDOW_HEIGHT;
int TILE_SIZE;

int grid[GRID_SIZE][GRID_SIZE] = {0};
bool gameStarted = false;
bool gameOver = false;
bool showHelp = false;
bool showOptions = false;
bool showCredits = false;
std::map<int, SDL_Texture*> fruitTextures;
int score = 0;
int highscore = 0;
int helpScrollOffset = 0;
bool isFullscreen = false;  // Start in windowed mode

// Forward declarations for screens used in main.
void draw_help_screen(SDL_Renderer* renderer, TTF_Font* font);
void draw_credits_screen(SDL_Renderer* renderer, TTF_Font* font);
void draw_options_screen(SDL_Renderer* renderer, TTF_Font* font);

// Load highscore from file
void loadHighscore() {
    std::ifstream infile("highscore.txt");
    if (infile.is_open()) {
        infile >> highscore;
        infile.close();
    } else {
        highscore = 0;
    }
}

// Save highscore to file
void saveHighscore() {
    std::ofstream outfile("highscore.txt");
    if (outfile.is_open()) {
        outfile << highscore;
        outfile.close();
    }
}

// Dummy function for file verification
void verifyFiles() {
    // Assume required files exist.
}

// Load fruit textures
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

// Recompute layout (7:1 ratio, clamped if needed)
void recomputeLayout(SDL_Window* window) {
    int actualW, actualH;
    SDL_GetWindowSize(window, &actualW, &actualH);

    // Use a 7:1 ratio (grid:sidebar) => total 8 parts
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

// Draw the start screen
void draw_start_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    SDL_Color textColor = {0, 0, 0, 255};

    SDL_Surface* titleSurface = TTF_RenderText_Solid(titleFont, "2048", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {
        (WINDOW_WIDTH - titleSurface->w) / 2,
        (WINDOW_HEIGHT / 3) - (titleSurface->h / 2),
        titleSurface->w,
        titleSurface->h
    };
    SDL_FreeSurface(titleSurface);
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_DestroyTexture(titleTexture);

    SDL_Surface* startSurface = TTF_RenderText_Solid(smallFont, "Press any key to start", textColor);
    SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
    SDL_Rect startRect = {
        (WINDOW_WIDTH - startSurface->w) / 2,
        titleRect.y + titleRect.h + 50,
        startSurface->w,
        startSurface->h
    };
    SDL_FreeSurface(startSurface);
    SDL_RenderCopy(renderer, startTexture, NULL, &startRect);
    SDL_DestroyTexture(startTexture);

    SDL_RenderPresent(renderer);
}

// Draw the game over screen with Restart and Quit buttons
void draw_game_over_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    SDL_Color textColor = {0, 0, 0, 255};

    // Draw title
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

    // Define button dimensions and positions
    int btnWidth = 200;
    int btnHeight = 50;
    int spacing = 20;
    int btnStartY = gameOverRect.y + gameOverRect.h + 50;
    SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight };
    SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight + spacing, btnWidth, btnHeight };

    // Draw Restart Button
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

    // Draw Quit Button
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

// Draw the sidebar with an Options button
void draw_sidebar(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Rect sidebarRect = {GAME_AREA_WIDTH, 0, SIDEBAR_WIDTH, WINDOW_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderFillRect(renderer, &sidebarRect);
    SDL_Color textColor = {0, 0, 0, 255};

    std::string scoreText = "Score: " + std::to_string(score);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = {GAME_AREA_WIDTH + 10, 50, scoreSurface->w, scoreSurface->h};
    SDL_FreeSurface(scoreSurface);
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_DestroyTexture(scoreTexture);

    std::string highscoreText = "Highscore: " + std::to_string(highscore);
    SDL_Surface* highSurface = TTF_RenderText_Solid(font, highscoreText.c_str(), textColor);
    SDL_Texture* highTexture = SDL_CreateTextureFromSurface(renderer, highSurface);
    SDL_Rect highRect = {GAME_AREA_WIDTH + 10, 100, highSurface->w, highSurface->h};
    SDL_FreeSurface(highSurface);
    SDL_RenderCopy(renderer, highTexture, NULL, &highRect);
    SDL_DestroyTexture(highTexture);

    // Draw Options button
    SDL_Rect optionButtonRect = {GAME_AREA_WIDTH + 10, WINDOW_HEIGHT - 60, SIDEBAR_WIDTH - 20, 50};
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
            optionSurface->w,
            optionSurface->h
        };
        SDL_FreeSurface(optionSurface);
        SDL_RenderCopy(renderer, optionTexture, NULL, &textRect);
        SDL_DestroyTexture(optionTexture);
    }
}

// Draw the main grid and sidebar
void draw_grid(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int value = grid[i][j];
            SDL_Rect rect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            SDL_SetRenderDrawColor(renderer, 205, 193, 180, 255);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
            if (value != 0 && fruitTextures.count(value)) {
                SDL_Texture* tex = fruitTextures[value];
                SDL_Rect destRect = {rect.x, rect.y, TILE_SIZE, TILE_SIZE};
                SDL_RenderCopy(renderer, tex, NULL, &destRect);
            }
        }
    }
    draw_sidebar(renderer, font);
    SDL_RenderPresent(renderer);
}

// Draw the Options screen with buttons for Help, Restart, Credits, Quit, and a Back button
void draw_options_screen(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    SDL_Color textColor = {0, 0, 0, 255};

    // Title "Options"
    SDL_Surface* titleSurf = TTF_RenderText_Solid(font, "Options", textColor);
    SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurf);
    SDL_Rect titleRect = {
        (WINDOW_WIDTH - titleSurf->w) / 2, 50,
        titleSurf->w, titleSurf->h
    };
    SDL_FreeSurface(titleSurf);
    SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
    SDL_DestroyTexture(titleTex);

    int btnWidth = 200;
    int btnHeight = 50;
    int spacing = 20;
    int startY = 120;
    SDL_Rect helpBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY, btnWidth, btnHeight };
    SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY + btnHeight + spacing, btnWidth, btnHeight };
    SDL_Rect creditBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY + 2 * (btnHeight + spacing), btnWidth, btnHeight };
    SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY + 3 * (btnHeight + spacing), btnWidth, btnHeight };
    SDL_Rect backBtn = { WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50 };

    // Draw Help button
    SDL_SetRenderDrawColor(renderer, 100, 100, 250, 255);
    SDL_RenderFillRect(renderer, &helpBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &helpBtn);
    SDL_Surface* helpTextSurf = TTF_RenderText_Solid(font, "Help", textColor);
    if (helpTextSurf) {
        SDL_Texture* helpTextTex = SDL_CreateTextureFromSurface(renderer, helpTextSurf);
        SDL_Rect helpTextRect = {
            helpBtn.x + (btnWidth - helpTextSurf->w) / 2,
            helpBtn.y + (btnHeight - helpTextSurf->h) / 2,
            helpTextSurf->w,
            helpTextSurf->h
        };
        SDL_FreeSurface(helpTextSurf);
        SDL_RenderCopy(renderer, helpTextTex, NULL, &helpTextRect);
        SDL_DestroyTexture(helpTextTex);
    }

    // Draw Restart button
    SDL_SetRenderDrawColor(renderer, 100, 250, 100, 255);
    SDL_RenderFillRect(renderer, &restartBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &restartBtn);
    SDL_Surface* restartTextSurf = TTF_RenderText_Solid(font, "Restart", textColor);
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

    // Draw Credits button
    SDL_SetRenderDrawColor(renderer, 250, 100, 100, 255);
    SDL_RenderFillRect(renderer, &creditBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &creditBtn);
    SDL_Surface* creditTextSurf = TTF_RenderText_Solid(font, "Credits", textColor);
    if (creditTextSurf) {
        SDL_Texture* creditTextTex = SDL_CreateTextureFromSurface(renderer, creditTextSurf);
        SDL_Rect creditTextRect = {
            creditBtn.x + (btnWidth - creditTextSurf->w) / 2,
            creditBtn.y + (btnHeight - creditTextSurf->h) / 2,
            creditTextSurf->w,
            creditTextSurf->h
        };
        SDL_FreeSurface(creditTextSurf);
        SDL_RenderCopy(renderer, creditTextTex, NULL, &creditTextRect);
        SDL_DestroyTexture(creditTextTex);
    }

    // Draw Quit button
    SDL_SetRenderDrawColor(renderer, 250, 100, 250, 255);
    SDL_RenderFillRect(renderer, &quitBtn);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &quitBtn);
    SDL_Surface* quitTextSurf = TTF_RenderText_Solid(font, "Quit", textColor);
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

    // Draw Back button
    SDL_Rect backButtonRect = {WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50};
    SDL_SetRenderDrawColor(renderer, 200, 200, 100, 255);
    SDL_RenderFillRect(renderer, &backButtonRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &backButtonRect);
    SDL_Surface* backTextSurf = TTF_RenderText_Solid(font, "Back", textColor);
    if (backTextSurf) {
        SDL_Texture* backTextTex = SDL_CreateTextureFromSurface(renderer, backTextSurf);
        SDL_Rect backTextRect = {
            backButtonRect.x + (backButtonRect.w - backTextSurf->w) / 2,
            backButtonRect.y + (backButtonRect.h - backTextSurf->h) / 2,
            backTextSurf->w,
            backTextSurf->h
        };
        SDL_FreeSurface(backTextSurf);
        SDL_RenderCopy(renderer, backTextTex, NULL, &backTextRect);
        SDL_DestroyTexture(backTextTex);
    }

    SDL_RenderPresent(renderer);
}

// Draw the help screen
void draw_help_screen(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    SDL_Color textColor = {0, 0, 0, 255};
    std::string instructions[] = {
        "Welcome to 2048 Fruits!",
        "",
        "Objective:",
        "Combine matching fruit tiles by sliding them",
        "with the arrow keys. When two tiles with the",
        "same value collide, they merge into one with",
        "double the points. Reach the 2048 tile while",
        "achieving the highest score.",
        "",
        "How to Play:",
        "- Use arrow keys to move tiles.",
        "- Identical tiles merge to double points.",
        "- Game over when no moves remain.",
        "- Highscore is saved persistently.",
        "",
        "Fruit to Points:",
        "Apple: 2",
        "Banana: 4",
        "Dragonfruit: 8",
        "Grape: 16",
        "Mango: 32",
        "Orange: 64",
        "Peach: 128",
        "Pineapple: 256",
        "Pomegranate: 512",
        "Strawberry: 1024",
        "Watermelon: 2048"
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
    SDL_Rect closeButtonRect = {WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50};
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

// Draw the credits screen
void draw_credits_screen(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);
    SDL_Color textColor = {0, 0, 0, 255};
    std::string credits = "Credits:\n\nDeveloped by Nguyen Hung Son";
    SDL_Surface* creditSurface = TTF_RenderText_Blended_Wrapped(font, credits.c_str(), textColor, WINDOW_WIDTH - 40);
    if (creditSurface) {
        SDL_Texture* creditTexture = SDL_CreateTextureFromSurface(renderer, creditSurface);
        SDL_Rect creditRect = {20, 20, creditSurface->w, creditSurface->h};
        SDL_FreeSurface(creditSurface);
        SDL_RenderCopy(renderer, creditTexture, NULL, &creditRect);
        SDL_DestroyTexture(creditTexture);
    }
    SDL_Rect backBtn = {WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50};
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
            if (grid[i][j] == 0) return false;
            if (i > 0 && grid[i][j] == grid[i - 1][j]) return false;
            if (i < GRID_SIZE - 1 && grid[i][j] == grid[i + 1][j]) return false;
            if (j > 0 && grid[i][j] == grid[i][j - 1]) return false;
            if (j < GRID_SIZE - 1 && grid[i][j] == grid[i][j + 1]) return false;
        }
    }
    return true;
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
                            grid[k - 1][j] *= 2;
                            score += grid[k - 1][j];
                            grid[k][j] = 0;
                            moved = true;
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
                            grid[k + 1][j] *= 2;
                            score += grid[k + 1][j];
                            grid[k][j] = 0;
                            moved = true;
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
                            grid[i][k - 1] *= 2;
                            score += grid[i][k - 1];
                            grid[i][k] = 0;
                            moved = true;
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
                            grid[i][k + 1] *= 2;
                            score += grid[i][k + 1];
                            grid[i][k] = 0;
                            moved = true;
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

    // Create a windowed, resizable window at 1000×700
    SDL_Window* window = SDL_CreateWindow(
        "2048 Fruits",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1000, 700,
        SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
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
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    recomputeLayout(window);
    loadTextures(renderer);
    TTF_Font* titleFont = TTF_OpenFont("arial.ttf", 48);
    TTF_Font* smallFont = TTF_OpenFont("arial.ttf", 24);
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
                if (!showOptions && !showHelp && !showCredits && !gameOver) {
                    if (!gameStarted) {
                        gameStarted = true;
                        gameOver = false;
                        initialize_grid();
                    } else {
                        move_tiles(e.key.keysym.sym);
                        if (is_game_over()) {
                            gameOver = true;
                        }
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
                }
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = e.button.x;
                    int mouseY = e.button.y;
                    if (!showOptions && !showHelp && !showCredits && !gameOver) {
                        SDL_Rect optionButtonRect = {GAME_AREA_WIDTH + 10, WINDOW_HEIGHT - 60, SIDEBAR_WIDTH - 20, 50};
                        if (mouseX >= optionButtonRect.x && mouseX <= optionButtonRect.x + optionButtonRect.w &&
                            mouseY >= optionButtonRect.y && mouseY <= optionButtonRect.y + optionButtonRect.h) {
                            showOptions = true;
                            continue;
                        }
                    } else if (showOptions) {
                        int btnWidth = 200;
                        int btnHeight = 50;
                        int spacing = 20;
                        int startY = 120;
                        SDL_Rect helpBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY, btnWidth, btnHeight };
                        SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY + btnHeight + spacing, btnWidth, btnHeight };
                        SDL_Rect creditBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY + 2 * (btnHeight + spacing), btnWidth, btnHeight };
                        SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, startY + 3 * (btnHeight + spacing), btnWidth, btnHeight };
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
                            showOptions = false;
                            continue;
                        } else if (mouseX >= creditBtn.x && mouseX <= creditBtn.x + creditBtn.w &&
                                   mouseY >= creditBtn.y && mouseY <= creditBtn.y + creditBtn.h) {
                            showCredits = true;
                            showOptions = false;
                            continue;
                        } else if (mouseX >= quitBtn.x && mouseX <= quitBtn.x + quitBtn.w &&
                                   mouseY >= quitBtn.y && mouseY <= quitBtn.y + quitBtn.h) {
                            quit = true;
                            continue;
                        } else if (mouseX >= backBtn.x && mouseX <= backBtn.x + backBtn.w &&
                                   mouseY >= backBtn.y && mouseY <= backBtn.y + backBtn.h) {
                            showOptions = false;
                            continue;
                        }
                    } else if (showHelp) {
                        SDL_Rect closeButtonRect = {WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50};
                        if (mouseX >= closeButtonRect.x && mouseX <= closeButtonRect.x + closeButtonRect.w &&
                            mouseY >= closeButtonRect.y && mouseY <= closeButtonRect.y + closeButtonRect.h) {
                            showHelp = false;
                            showOptions = true;
                            continue;
                        }
                    } else if (showCredits) {
                        SDL_Rect backButtonRect = {WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50};
                        if (mouseX >= backButtonRect.x && mouseX <= backButtonRect.x + backButtonRect.w &&
                            mouseY >= backButtonRect.y && mouseY <= backButtonRect.y + backButtonRect.h) {
                            showCredits = false;
                            showOptions = true;
                            continue;
                        }
                    } else if (gameOver) {
                        int btnWidth = 200;
                        int btnHeight = 50;
                        int spacing = 20;
                        int btnStartY = (WINDOW_HEIGHT / 4) + 100;
                        SDL_Rect restartBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY, btnWidth, btnHeight };
                        SDL_Rect quitBtn = { (WINDOW_WIDTH - btnWidth) / 2, btnStartY + btnHeight + spacing, btnWidth, btnHeight };
                        if (mouseX >= restartBtn.x && mouseX <= restartBtn.x + restartBtn.w &&
                            mouseY >= restartBtn.y && mouseY <= restartBtn.y + restartBtn.h) {
                            initialize_grid();
                            gameStarted = true;
                            gameOver = false;
                            continue;
                        } else if (mouseX >= quitBtn.x && mouseX <= quitBtn.x + quitBtn.w &&
                                   mouseY >= quitBtn.y && mouseY <= quitBtn.y + quitBtn.h) {
                            quit = true;
                            continue;
                        }
                    }
                }
            }
            else if (showOptions && e.type == SDL_MOUSEWHEEL) {
                helpScrollOffset -= e.wheel.y * 20;
            }
        }

        if (showOptions) {
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
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
















