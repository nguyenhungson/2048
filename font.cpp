#include "globals.h"
#include "font.h"
#include <iostream>
#include <SDL_ttf.h>

bool initFont(){
    titleFont   = TTF_OpenFont("Font/doodle.ttf", 72);
    if (!titleFont){
        std::cerr << "Failed to load title font: " << TTF_GetError() << "\n";
    }
    smallFont   = TTF_OpenFont("Font/doodle.ttf", 42);
    if (!smallFont){
        std::cerr << "Failed to load small font: " << TTF_GetError() << "\n";
    }
    buttonFont  = TTF_OpenFont("Font/doodle.ttf", 36);
    if (!buttonFont){
        std::cerr << "Failed to load button font: " << TTF_GetError() << "\n";
    }
    boosterFont  = TTF_OpenFont("Font/doodle.ttf", 28);
    if (!boosterFont){
        std::cerr << "Failed to load booster font: " << TTF_GetError() << "\n";
    }
    valueFont  = TTF_OpenFont("Font/doodle.ttf", 20);
    if (!valueFont){
        std::cerr << "Failed to load value font: " << TTF_GetError() << "\n";
    }
    return true;
}

void freeAllFont(){
    if (titleFont) TTF_CloseFont(titleFont);
    if (smallFont) TTF_CloseFont(smallFont);
    if (buttonFont) TTF_CloseFont(buttonFont);
    if (boosterFont) TTF_CloseFont(boosterFont);
    if (valueFont) TTF_CloseFont(valueFont);
}
