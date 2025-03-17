#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <map>

// Constants for window, grid and tile size
const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 400;
const int GRID_SIZE = 4;
const int TILE_SIZE = WINDOW_WIDTH / GRID_SIZE;

// Global game state variables
int grid[GRID_SIZE][GRID_SIZE] = {0};
bool gameStarted = false;
bool gameOver = false;
std::map<int, SDL_Texture*> fruitTextures;

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

void draw_grid(SDL_Renderer* renderer) {
    // Clear background
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);

    // Loop through rows and columns
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int value = grid[i][j];

            // Draw tile background
            SDL_Rect rect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            SDL_SetRenderDrawColor(renderer, 205, 193, 180, 255);
            SDL_RenderFillRect(renderer, &rect);

            // Draw tile outline
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);

            // Draw fruit image if available for the tile value
            if (value != 0 && fruitTextures.count(value)) {
                SDL_Texture* tex = fruitTextures[value];
                int texWidth, texHeight;
                SDL_QueryTexture(tex, NULL, NULL, &texWidth, &texHeight);

                // Preserve aspect ratio by computing a scale factor
                float ratioW = static_cast<float>(TILE_SIZE) / texWidth;
                float ratioH = static_cast<float>(TILE_SIZE) / texHeight;
                float ratio = (ratioW < ratioH) ? ratioW : ratioH;

                // Final scaled width and height
                int finalW = static_cast<int>(texWidth * ratio);
                int finalH = static_cast<int>(texHeight * ratio);

                // Center the image in the tile
                SDL_Rect destRect = {
                    rect.x + (TILE_SIZE - finalW) / 2,
                    rect.y + (TILE_SIZE - finalH) / 2,
                    finalW,
                    finalH
                };

                // Render the scaled texture
                SDL_RenderCopy(renderer, tex, NULL, &destRect);
            }
        }
    }
    SDL_RenderPresent(renderer);
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

void add_random_tile() {
    int empty_cells = 0;
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            if (grid[i][j] == 0) empty_cells++;

    if (empty_cells == 0) return;

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
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            grid[i][j] = 0;  // Clear the grid

    for (int i = 0; i < 2; i++) add_random_tile();  // Add initial tiles
}

bool is_game_over() {
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) return false;
            if (i > 0 && grid[i][j] == grid[i - 1][j]) return false;
            if (i < GRID_SIZE - 1 && grid[i][j] == grid[i + 1][j]) return false;
            if (j > 0 && grid[i][j] == grid[i][j - 1]) return false;
            if (j < GRID_SIZE - 1 && grid[i][j] == grid[i][j + 1]) return false;
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

    // Texture loading
    loadTextures(renderer);

    // Font loading
    TTF_Font* titleFont = TTF_OpenFont("arial.ttf", 48);
    TTF_Font* smallFont = TTF_OpenFont("arial.ttf", 24);
    if (!titleFont || !smallFont) {
        std::cerr << "Font loading error: " << TTF_GetError() << "\n";
    }

    // Main loop
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            else if (e.type == SDL_KEYDOWN) {
                if (!gameStarted) {
                    gameStarted = true;
                    gameOver = false;
                    initialize_grid();
                } else if (gameOver) {
                    gameOver = false;
                    initialize_grid();
                } else {
                    move_tiles(e.key.keysym.sym);
                    if (is_game_over()) gameOver = true;
                }
            }
        }

        if (!gameStarted) draw_start_screen(renderer, titleFont, smallFont);
        else if (gameOver) draw_game_over_screen(renderer, titleFont, smallFont);
        else draw_grid(renderer);
    }

    // Cleanup
    for (auto& pair : fruitTextures) SDL_DestroyTexture(pair.second);
    TTF_CloseFont(titleFont);
    TTF_CloseFont(smallFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}

