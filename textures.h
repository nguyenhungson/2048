#ifndef TEXTURES_H_INCLUDED
#define TEXTURES_H_INCLUDED

#include <SDL.h>
#include <string>
#include <map>

// Function prototypes for loading and cleaning up textures.
bool loadAllTextures(SDL_Renderer* renderer);
void freeAllTextures();

#endif // TEXTURES_H_INCLUDED
