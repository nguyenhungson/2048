#ifndef GAME_H
#define GAME_H

#include <SDL.h>

void add_random_tile();

void initialize_grid();

void move_tiles(SDL_Keycode key);

bool is_game_over();

bool is_game_won();

void loadHighscore();

void saveHighscore();

#endif // GAME_H
