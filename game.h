#ifndef GAME_H
#define GAME_H

#include <SDL.h>


void add_random_tile();
/** Initialize the 4×4 grid with two random tiles. */
void initialize_grid();

/** Move tiles based on the arrow key pressed. */
void move_tiles(SDL_Keycode key);

/** Check if the game is over (no more moves). */
bool is_game_over();

/** Check if the player has reached 2048. */
bool is_game_won();

/** Load highscore from file. */
void loadHighscore();

/** Save highscore to file. */
void saveHighscore();

#endif // GAME_H
