#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL.h>
#include <SDL_ttf.h>

void recomputeLayout(SDL_Window* window);

void draw_start_screen(SDL_Renderer* renderer);

void draw_sidebar(SDL_Renderer* renderer, TTF_Font* valueFont, TTF_Font* smallFont);

void draw_grid(SDL_Renderer* renderer, TTF_Font* font);

void draw_help_screen(SDL_Renderer* renderer, TTF_Font* titleFont, TTF_Font* smallFont);

void draw_credits_screen(SDL_Renderer* renderer,
                         TTF_Font* titleFont,
                         TTF_Font* smallFont,
                         TTF_Font* buttonFont);

void draw_options_screen(SDL_Renderer* renderer,
                         TTF_Font* buttonFont,
                         TTF_Font* titleFont);

void draw_game_over_screen(SDL_Renderer* renderer,
                           TTF_Font* titleFont,
                           TTF_Font* smallFont);

void draw_win_screen(SDL_Renderer* renderer,
                     TTF_Font* titleFont,
                     TTF_Font* smallFont);

void drawCloudButtonWithText(SDL_Renderer* renderer,
                             SDL_Texture* cloudTex,
                             const SDL_Rect &btnRect,
                             const char* text,
                             TTF_Font* font);

#endif // GRAPHICS_H
