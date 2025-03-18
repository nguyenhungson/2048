#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <map>
#include <fstream>  // For persistent highscore

// Constants for game area and sidebar
const int GAME_AREA_WIDTH = 400;
const int SIDEBAR_WIDTH = 200;
const int WINDOW_WIDTH = GAME_AREA_WIDTH + SIDEBAR_WIDTH; // 600
const int WINDOW_HEIGHT = 400;
const int GRID_SIZE = 4;
const int TILE_SIZE = GAME_AREA_WIDTH / GRID_SIZE; // 100

// Global game state variables
int grid[GRID_SIZE][GRID_SIZE] = {0};
bool gameStarted = false;
bool gameOver = false;
bool showHelp = false; // Whether to display the help screen
std::map<int, SDL_Texture*> fruitTextures;
int score = 0;
int highscore = 0;

// NEW: Global scroll offset for the help screen
int helpScrollOffset = 0;

// Load the highscore from a file
void loadHighscore() {
    std::ifstream infile("highscore.txt");
    if (infile.is_open()) {
        infile >> highscore;
        infile.close();
    } else {
        highscore = 0;
    }
}

// Save the highscore to a file
void saveHighscore() {
    std::ofstream outfile("highscore.txt");
    if (outfile.is_open()) {
        outfile << highscore;
        outfile.close();
    }
}

// Dummy function for file verification (update if needed)
void verifyFiles() {

}

// Texture loading for fruit images
void loadTextures(SDL_Renderer* renderer) {
    std::string fruitNames[] = {"apple", "banana", "dragonfruit", "grape", "mango", "orange", "peach", "pineapple", "pomegranate", "strawberry", "watermelon"};
    int values[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
    for (int i = 0; i < 11; i++) {
        std::string path = "Fruit/" + fruitNames[i] + ".jpg";
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            std::cerr << "Failed to load image: " << path << " Error: " << IMG_GetError() << std::endl;
            continue;
        }
        fruitTextures[values[i]] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void draw_start_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {0, 0, 0, 255};

    SDL_Surface* titleSurface = TTF_RenderText_Solid(titleFont, "2048", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {WINDOW_WIDTH / 2 - titleSurface->w / 2, 120, titleSurface->w, titleSurface->h};
    SDL_FreeSurface(titleSurface);
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_DestroyTexture(titleTexture);

    SDL_Surface* startSurface = TTF_RenderText_Solid(smallFont, "Press any key to start", textColor);
    SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
    SDL_Rect startRect = {WINDOW_WIDTH / 2 - startSurface->w / 2, 200, startSurface->w, startSurface->h};
    SDL_FreeSurface(startSurface);
    SDL_RenderCopy(renderer, startTexture, NULL, &startRect);
    SDL_DestroyTexture(startTexture);

    SDL_RenderPresent(renderer);
}

void draw_game_over_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {0, 0, 0, 255};

    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(titleFont, "Game Over!", textColor);
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
    SDL_Rect gameOverRect = {WINDOW_WIDTH / 2 - gameOverSurface->w / 2, 120, gameOverSurface->w, gameOverSurface->h};
    SDL_FreeSurface(gameOverSurface);
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
    SDL_DestroyTexture(gameOverTexture);

    SDL_Surface* restartSurface = TTF_RenderText_Solid(smallFont, "Press any key to restart", textColor);
    SDL_Texture* restartTexture = SDL_CreateTextureFromSurface(renderer, restartSurface);
    SDL_Rect restartRect = {WINDOW_WIDTH / 2 - restartSurface->w / 2, 200, restartSurface->w, restartSurface->h};
    SDL_FreeSurface(restartSurface);
    SDL_RenderCopy(renderer, restartTexture, NULL, &restartRect);
    SDL_DestroyTexture(restartTexture);

    SDL_RenderPresent(renderer);
}

// Draw the sidebar with score and highscore, plus the clickable Help button
void draw_sidebar(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Rect sidebarRect = {GAME_AREA_WIDTH, 0, SIDEBAR_WIDTH, WINDOW_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // Gray background
    SDL_RenderFillRect(renderer, &sidebarRect);

    SDL_Color textColor = {0, 0, 0, 255};

    // Render score text
    std::string scoreText = "Score: " + std::to_string(score);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = {GAME_AREA_WIDTH + 10, 50, scoreSurface->w, scoreSurface->h};
    SDL_FreeSurface(scoreSurface);
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_DestroyTexture(scoreTexture);

    // Render highscore text
    std::string highscoreText = "Highscore: " + std::to_string(highscore);
    SDL_Surface* highSurface = TTF_RenderText_Solid(font, highscoreText.c_str(), textColor);
    SDL_Texture* highTexture = SDL_CreateTextureFromSurface(renderer, highSurface);
    SDL_Rect highRect = {GAME_AREA_WIDTH + 10, 100, highSurface->w, highSurface->h};
    SDL_FreeSurface(highSurface);
    SDL_RenderCopy(renderer, highTexture, NULL, &highRect);
    SDL_DestroyTexture(highTexture);

    // Draw the Help Button
    SDL_Rect helpButtonRect = {GAME_AREA_WIDTH + 10, WINDOW_HEIGHT - 60, SIDEBAR_WIDTH - 20, 50};
    SDL_SetRenderDrawColor(renderer, 100, 100, 250, 255); // Blue button
    SDL_RenderFillRect(renderer, &helpButtonRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Button border
    SDL_RenderDrawRect(renderer, &helpButtonRect);

    // Render "Help" text on the button
    std::string helpText = "Help";
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* helpSurface = TTF_RenderText_Solid(font, helpText.c_str(), white);
    if (helpSurface) {
        SDL_Texture* helpTexture = SDL_CreateTextureFromSurface(renderer, helpSurface);
        SDL_Rect textRect = {
            helpButtonRect.x + (helpButtonRect.w - helpSurface->w) / 2,
            helpButtonRect.y + (helpButtonRect.h - helpSurface->h) / 2,
            helpSurface->w, helpSurface->h
        };
        SDL_FreeSurface(helpSurface);
        SDL_RenderCopy(renderer, helpTexture, NULL, &textRect);
        SDL_DestroyTexture(helpTexture);
    }
}

// Draw game grid (left area) and sidebar
void draw_grid(SDL_Renderer* renderer, TTF_Font* font) {
    // Clear the entire window
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);

    // Draw the game grid in the game area (left side)
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int value = grid[i][j];
            SDL_Rect rect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            SDL_SetRenderDrawColor(renderer, 205, 193, 180, 255);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);

            // Draw fruit image if available for the tile value
            if (value != 0 && fruitTextures.count(value)) {
                SDL_Texture* tex = fruitTextures[value];
                int texWidth, texHeight;
                SDL_QueryTexture(tex, NULL, NULL, &texWidth, &texHeight);
                float ratioW = static_cast<float>(TILE_SIZE) / texWidth;
                float ratioH = static_cast<float>(TILE_SIZE) / texHeight;
                float ratio = (ratioW < ratioH) ? ratioW : ratioH;
                int finalW = static_cast<int>(texWidth * ratio);
                int finalH = static_cast<int>(texHeight * ratio);
                SDL_Rect destRect = {
                    rect.x + (TILE_SIZE - finalW) / 2,
                    rect.y + (TILE_SIZE - finalH) / 2,
                    finalW,
                    finalH
                };
                SDL_RenderCopy(renderer, tex, NULL, &destRect);
            }
        }
    }
    // Draw the sidebar with score information
    draw_sidebar(renderer, font);
    SDL_RenderPresent(renderer);
}

// Draw the Help screen with instructions and a Close button, applying scrolling
void draw_help_screen(SDL_Renderer* renderer, TTF_Font* font) {
    // Clear background
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
        "Watermelon: 2048",
        "",
        "Creators: Nguyen Hung Son"
    };

    // Compute line height using TTF_FontLineSkip
    int lineHeight = TTF_FontLineSkip(font);
    int numLines = sizeof(instructions) / sizeof(instructions[0]);
    int totalHeight = numLines * lineHeight;

    // Clamp helpScrollOffset so it doesn't scroll too far
    if (helpScrollOffset < 0) helpScrollOffset = 0;
    if (helpScrollOffset > totalHeight - WINDOW_HEIGHT)
        helpScrollOffset = totalHeight - WINDOW_HEIGHT;

    // Start drawing text with the scroll offset applied
    int y = 20 - helpScrollOffset; // starting y position
    for (int i = 0; i < numLines; i++) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, instructions[i].c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {20, y, textSurface->w, textSurface->h};
            SDL_FreeSurface(textSurface);
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
            y += lineHeight; // use the line height for spacing
        }
    }

    // Draw the Close Button on the Help screen
    SDL_Rect closeButtonRect = {WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50};
    SDL_SetRenderDrawColor(renderer, 250, 100, 100, 255); // Red button
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

    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            if (grid[i][j] == 0 && target-- == 0) {
                grid[i][j] = value;
                return;
            }
}

void initialize_grid() {
    srand(time(0));
    score = 0; // Reset score when starting a new game
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            grid[i][j] = 0;  // Clear the grid

    for (int i = 0; i < 2; i++)
        add_random_tile();  // Add initial tiles
}

bool is_game_over() {
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0)
                return false;
            if (i > 0 && grid[i][j] == grid[i - 1][j])
                return false;
            if (i < GRID_SIZE - 1 && grid[i][j] == grid[i + 1][j])
                return false;
            if (j > 0 && grid[i][j] == grid[i][j - 1])
                return false;
            if (j < GRID_SIZE - 1 && grid[i][j] == grid[i][j + 1])
                return false;
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
                            score += grid[k - 1][j]; // Update score
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
                            score += grid[k + 1][j]; // Update score
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
                            score += grid[i][k - 1]; // Update score
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
                            score += grid[i][k + 1]; // Update score
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
            saveHighscore(); // Persist new highscore
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize subsystems with error checking
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

    // Create window
    SDL_Window* window = SDL_CreateWindow("2048 Fruits",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load textures and fonts
    loadTextures(renderer);
    TTF_Font* titleFont = TTF_OpenFont("arial.ttf", 48);
    TTF_Font* smallFont = TTF_OpenFont("arial.ttf", 24);
    if (!titleFont || !smallFont) {
        std::cerr << "Font loading error: " << TTF_GetError() << "\n";
    }

    // Load persistent highscore
    loadHighscore();

    // Main game loop
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = e.button.x;
                    int mouseY = e.button.y;
                    if (!showHelp) {
                        // Check if the click is inside the Help button in the sidebar
                        SDL_Rect helpButtonRect = {GAME_AREA_WIDTH + 10, WINDOW_HEIGHT - 60, SIDEBAR_WIDTH - 20, 50};
                        if (mouseX >= helpButtonRect.x && mouseX <= helpButtonRect.x + helpButtonRect.w &&
                            mouseY >= helpButtonRect.y && mouseY <= helpButtonRect.y + helpButtonRect.h) {
                            showHelp = true;
                            continue; // Skip further processing for this event
                        }
                    }
                    else {
                        // If the Help screen is showing, check for a click on the Close button
                        SDL_Rect closeButtonRect = {WINDOW_WIDTH - 110, WINDOW_HEIGHT - 60, 100, 50};
                        if (mouseX >= closeButtonRect.x && mouseX <= closeButtonRect.x + closeButtonRect.w &&
                            mouseY >= closeButtonRect.y && mouseY <= closeButtonRect.y + closeButtonRect.h) {
                            showHelp = false;
                            continue;
                        }
                    }
                }
            }
            // NEW: Handle scrolling events in Help screen
            else if (showHelp && e.type == SDL_MOUSEWHEEL) {
                // On Windows, e.wheel.y is usually 1 for scroll up, -1 for scroll down.
                helpScrollOffset -= e.wheel.y * 20; // adjust scroll speed as desired
            }
            else if (showHelp && e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP) {
                    helpScrollOffset -= 20;
                }
                else if (e.key.keysym.sym == SDLK_DOWN) {
                    helpScrollOffset += 20;
                }
                // Also allow closing help with Escape
                else if (e.key.keysym.sym == SDLK_ESCAPE) {
                    showHelp = false;
                }
            }
            else if (e.type == SDL_KEYDOWN) {
                // Existing key handling for game play
                if (!gameStarted) {
                    gameStarted = true;
                    gameOver = false;
                    initialize_grid();
                }
                else if (gameOver) {
                    gameOver = false;
                    initialize_grid();
                }
                else {
                    move_tiles(e.key.keysym.sym);
                    if (is_game_over())
                        gameOver = true;
                }
            }
        }

        // Rendering based on state
        if (showHelp)
            draw_help_screen(renderer, smallFont);
        else if (!gameStarted)
            draw_start_screen(renderer, titleFont, smallFont);
        else if (gameOver)
            draw_game_over_screen(renderer, titleFont, smallFont);
        else
            draw_grid(renderer, smallFont);
    }

    // Cleanup resources
    for (auto& pair : fruitTextures)
        SDL_DestroyTexture(pair.second);
    TTF_CloseFont(titleFont);
    TTF_CloseFont(smallFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}



