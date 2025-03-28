#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL.h>
#include <SDL_ttf.h>

/** Recompute layout (grid sizes, window dims, etc.) */
void recomputeLayout(SDL_Window* window);

/** Draw the start screen (like your “Press any key to start”). */
void draw_start_screen(SDL_Renderer* renderer);

void draw_sidebar(SDL_Renderer* renderer, TTF_Font* font);

/** Draw the main 4×4 grid plus the sidebar. */
void draw_grid(SDL_Renderer* renderer, TTF_Font* font);

/** Draw the help screen. */
void draw_help_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont);

/** Draw the credits screen. */
void draw_credits_screen(SDL_Renderer* renderer,
                         TTF_Font* titleFont,
                         TTF_Font* smallFont,
                         TTF_Font* buttonFont);

/** Draw the options screen. */
void draw_options_screen(SDL_Renderer* renderer,
                         TTF_Font* buttonFont,
                         TTF_Font* titleFont);

/** Draw the game-over screen. */
void draw_game_over_screen(SDL_Renderer* renderer,
                           TTF_Font* titleFont,
                           TTF_Font* smallFont);

/** Draw the “you win” screen. */
void draw_win_screen(SDL_Renderer* renderer,
                     TTF_Font* titleFont,
                     TTF_Font* smallFont);
void drawCloudButtonWithText(SDL_Renderer* renderer,
                             SDL_Texture* cloudTex,
                             const SDL_Rect &btnRect,
                             const char* text,
                             TTF_Font* font);
#endif // GRAPHICS_H
