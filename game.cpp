#include "game.h"
#include "globals.h"
#include "audio.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

// Global variables
int GRID_SIZE = 4;
int grid[4][4] = {0};
bool lock2048 = false;
int score = 0;
int incrementscore = 0;
int highscore = 0;
bool newHighscoreAchieved = false;

void add_random_tile()
{
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

void loadHighscore()
{
    std::ifstream infile("highscore.txt");
    if (infile.is_open()) {
        infile >> highscore;
    }
}

void saveHighscore()
{
    std::ofstream outfile("highscore.txt");
    if (outfile.is_open()) {
        outfile << highscore;
    }
}

void initialize_grid()
{
    srand((unsigned)time(nullptr));
    score = 0;
    newHighscoreAchieved = false;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 0;
        }
    }
    add_random_tile();
    add_random_tile();
}

void move_tiles(SDL_Keycode key)
{
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

bool is_game_over()
{
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 2048) {
                return true;
            }
        }
    }
    return false;
}

bool is_game_won()
{
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            if (grid[i][j] == 2048)
                return true;
    return false;
}
