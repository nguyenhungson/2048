#include "game.h"
#include "globals.h"
#include "audio.h"
#include "boosters.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <map>

void add_random_tile() {
    int empty_cells = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0)
                empty_cells++;
        }
    }
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

void add_random_blocker() {
    int emptyCells = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) {
                emptyCells++;
            }
        }
    }
    if (emptyCells == 0) {
        return;
    }
    if (freezeActive){
        emptyCells = 1e7;
    }
    int target = rand() % (emptyCells);
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0 && target-- == 0) {
                grid[i][j] = BLOCKER_VALUE;
                return;
            }
        }
    }
}


void loadHighscore()
{
    std::ifstream infile("assets/text/highscore.txt");
    if (infile.is_open()) {
        infile >> highscore;
    }
}

void saveHighscore()
{
    std::ofstream outfile("assets/text/highscore.txt");
    if (outfile.is_open()) {
        outfile << highscore;
    }
}

void initialize_grid() {
    std::cerr << "initialize_grid() start" << std::endl;
    srand((unsigned)time(nullptr));
    score = 0;
    newHighscoreAchieved = false;
    boosterActive = false;
    hammerActive = false;
    freezeActive = false;
    tsunamiActive = false;
    currentBooster = {100, 0};
    boosterStartTime = 0;
    for (const auto &pair : boosterSettings) {
        boosterActivated[pair.first] = false;
    }
    newHighscoreAchieved = false;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 0;
        }
    }
    std::cerr << "Before add_random_tile()" << std::endl;
    add_random_tile();
    std::cerr << "initialize_grid() end" << std::endl;
}

void move_tiles(SDL_Keycode key)
{
    incrementscore = 0;
    bool moved = false;
    switch (key) {
        case SDLK_UP:
            for (int j = 0; j < GRID_SIZE; j++) {
                for (int i = 1; i < GRID_SIZE; i++) {
                    if (grid[i][j] != 0 && grid[i][j] != BLOCKER_VALUE) {
                        int k = i;
                        while (k > 0 && grid[k - 1][j] == 0) {
                            if (grid[k - 1][j] == BLOCKER_VALUE)
                                break;
                            grid[k - 1][j] = grid[k][j];
                            grid[k][j] = 0;
                            k--;
                            moved = true;
                        }
                        if (k > 0 && grid[k - 1][j] == grid[k][j]) {
                            if (grid[k][j] == 2048 && lock2048) {
                            } else {
                                grid[k - 1][j] *= 2;
                                int pointsGained = grid[k - 1][j];
                                if (boosterSettings.count(grid[k - 1][j]) && !boosterActivated[grid[k - 1][j]]) {
                                    currentBooster = boosterSettings[grid[k - 1][j]];
                                    boosterActive = true;
                                    boosterStartTime = SDL_GetTicks();
                                    boosterActivated[grid[k - 1][j]] = true;
                                }
                                if (boosterActive){
                                    pointsGained = static_cast<int>(pointsGained * (currentBooster.multiplier / 100.0));
                                    incrementscore += pointsGained;
                                    score += pointsGained;
                                }
                                else{
                                    incrementscore += pointsGained;
                                    score += pointsGained;
                                }
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
                    if (grid[i][j] != 0 && grid[i][j] != BLOCKER_VALUE) {
                        int k = i;
                        while (k < GRID_SIZE - 1 && grid[k + 1][j] == 0) {
                            if (grid[k + 1][j] == BLOCKER_VALUE)
                                break;
                            grid[k + 1][j] = grid[k][j];
                            grid[k][j] = 0;
                            k++;
                            moved = true;
                        }
                        if (k < GRID_SIZE - 1 && grid[k + 1][j] == grid[k][j]) {
                            if (grid[k][j] == 2048 && lock2048) {
                            } else {
                                grid[k + 1][j] *= 2;
                                int pointsGained = grid[k + 1][j];
                                if (boosterSettings.count(grid[k + 1][j]) && !boosterActivated[grid[k + 1][j]]) {
                                    currentBooster = boosterSettings[grid[k + 1][j]];
                                    boosterActive = true;
                                    boosterStartTime = SDL_GetTicks();
                                    boosterActivated[grid[k + 1][j]] = true;
                                }
                                if (boosterActive){
                                    pointsGained = static_cast<int>(pointsGained * (currentBooster.multiplier / 100.0));
                                    incrementscore += pointsGained;
                                    score += pointsGained;
                                }
                                else{
                                    incrementscore += pointsGained;
                                    score += pointsGained;
                                }
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
                    if (grid[i][j] != 0 && grid[i][j] != BLOCKER_VALUE) {
                        int k = j;
                        while (k > 0 && grid[i][k - 1] == 0) {
                            if (grid[i][k - 1] == BLOCKER_VALUE)
                                break;
                            grid[i][k - 1] = grid[i][k];
                            grid[i][k] = 0;
                            k--;
                            moved = true;
                        }
                        if (k > 0 && grid[i][k - 1] == grid[i][k]) {
                            if (grid[i][k] == 2048 && lock2048) {
                            } else {
                                grid[i][k - 1] *= 2;
                                int pointsGained = grid[i][k - 1];
                                if (boosterSettings.count(grid[i][k - 1]) && !boosterActivated[grid[i][k - 1]]) {
                                    currentBooster = boosterSettings[grid[i][k - 1]];
                                    boosterActive = true;
                                    boosterStartTime = SDL_GetTicks();
                                    boosterActivated[grid[i][k - 1]] = true;
                                }
                                if (boosterActive){
                                    pointsGained = static_cast<int>(pointsGained * (currentBooster.multiplier / 100.0));
                                    incrementscore += pointsGained;
                                    score += pointsGained;
                                }
                                else{
                                    incrementscore += pointsGained;
                                    score += pointsGained;
                                }
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
                    if (grid[i][j] != 0 && grid[i][j] != BLOCKER_VALUE) {
                        int k = j;
                        while (k < GRID_SIZE - 1 && grid[i][k + 1] == 0) {
                            if (grid[i][k + 1] == BLOCKER_VALUE)
                                break;
                            grid[i][k + 1] = grid[i][k];
                            grid[i][k] = 0;
                            k++;
                            moved = true;
                        }
                        if (k < GRID_SIZE - 1 && grid[i][k + 1] == grid[i][k]) {
                            if (grid[i][k] == 2048 && lock2048) {
                            } else {
                                grid[i][k + 1] *= 2;
                                int pointsGained = grid[i][k + 1];
                                if (boosterSettings.count(grid[i][k + 1]) && !boosterActivated[grid[i][k + 1]]) {
                                    currentBooster = boosterSettings[grid[i][k + 1]];
                                    boosterActive = true;
                                    boosterStartTime = SDL_GetTicks();
                                    boosterActivated[grid[i][k + 1]] = true;
                                }
                                if (boosterActive){
                                    pointsGained = static_cast<int>(pointsGained * (currentBooster.multiplier / 100.0));
                                    incrementscore += pointsGained;
                                    score += pointsGained;
                                }
                                else{
                                    incrementscore += pointsGained;
                                    score += pointsGained;
                                }
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
        if (rand() % 100 < 5) { // 20% chance
            add_random_blocker();
        }
        if (score > highscore) {
            highscore = score;
            saveHighscore();
            if (!newHighscoreAchieved && !congratsShown) {
                newHighscoreAchieved = true;
                congratsShown = true;
                newHighscoreTime = SDL_GetTicks();
                if (congratsMusic) {
                    Mix_HookMusicFinished(MusicFinishedCallback);
                    Mix_PlayMusic(congratsMusic, 1);
                }
            }
        }
        if (is_game_won() && !lock2048) {
            if (!gameWon) {
                int numTextures = gamewinTextures.size();
                currentWinIndex = (rand() % numTextures) + 1;
                gameWon = true;
                Mix_PlayMusic(gameWinMusic, -1);
            }
        }
        if (is_game_over()) {
            if (!gameOver) {
                Mix_HookMusicFinished(NULL);
                Mix_HaltMusic();
                int numTextures = gameoverTextures.size();
                currentGameoverIndex = (rand() % numTextures) + 1;
                gameOver = true;
                Mix_PlayChannel(-1, gameOverSound, 0);
            }
        }
    }
}

bool is_game_over() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0)
                return false;

            if (grid[i][j] == BLOCKER_VALUE)
                continue;

            if (i > 0 && grid[i - 1][j] != BLOCKER_VALUE && grid[i][j] == grid[i - 1][j])
                return false;
            if (i < GRID_SIZE - 1 && grid[i + 1][j] != BLOCKER_VALUE && grid[i][j] == grid[i + 1][j])
                return false;
            if (j > 0 && grid[i][j - 1] != BLOCKER_VALUE && grid[i][j] == grid[i][j - 1])
                return false;
            if (j < GRID_SIZE - 1 && grid[i][j + 1] != BLOCKER_VALUE && grid[i][j] == grid[i][j + 1])
                return false;
        }
    }
    return true;
}

bool is_game_won() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 2048)
                return true;
        }
    }
    return false;
}
