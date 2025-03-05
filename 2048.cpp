#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 400;
const int GRID_SIZE = 4;
const int TILE_SIZE = WINDOW_WIDTH / GRID_SIZE;

int grid[GRID_SIZE][GRID_SIZE] = {0};
bool gameStarted = false;
bool gameOver = false;

// Function to draw the grid using SDL
void draw_grid(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int value = grid[i][j];
            SDL_Rect rect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};

            // Set tile color based on value
            switch (value) {
                case 0: SDL_SetRenderDrawColor(renderer, 205, 193, 180, 255); break;
                case 2: SDL_SetRenderDrawColor(renderer, 238, 228, 218, 255); break;
                case 4: SDL_SetRenderDrawColor(renderer, 237, 224, 200, 255); break;
                case 8: SDL_SetRenderDrawColor(renderer, 242, 177, 121, 255); break;
                case 16: SDL_SetRenderDrawColor(renderer, 245, 149, 99, 255); break;
                case 32: SDL_SetRenderDrawColor(renderer, 246, 124, 95, 255); break;
                case 64: SDL_SetRenderDrawColor(renderer, 246, 94, 59, 255); break;
                default: SDL_SetRenderDrawColor(renderer, 237, 207, 114, 255); break;
            }

            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);

            // Render the number on the tile
            if (value != 0) {
                std::string text = std::to_string(value);
                SDL_Color textColor = {0, 0, 0, 255}; // Black color for text

                SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

                int textWidth = surface->w;
                int textHeight = surface->h;
                SDL_FreeSurface(surface);

                SDL_Rect textRect = {
                    rect.x + (TILE_SIZE - textWidth) / 2,
                    rect.y + (TILE_SIZE - textHeight) / 2,
                    textWidth,
                    textHeight
                };

                SDL_RenderCopy(renderer, texture, nullptr, &textRect);
                SDL_DestroyTexture(texture);
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

// Function to handle key presses and move tiles
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
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("2048", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* titleFont = TTF_OpenFont("arial.ttf", 48);
    TTF_Font* smallFont = TTF_OpenFont("arial.ttf", 24);

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
                // Restart the game on key press
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
    else draw_grid(renderer, smallFont);
}

    TTF_CloseFont(titleFont);
    TTF_CloseFont(smallFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

