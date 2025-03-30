#include "textures.h"
#include "globals.h"
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <map>

bool loadAllTextures(SDL_Renderer* renderer)
{
    SDL_Surface* startSurface = IMG_Load("assets/backgrounds and textures/startbg.png");
    if (!startSurface)
        std::cerr << "Failed to load start PNG file: " << IMG_GetError() << "\n";

    SDL_Surface* sidebarSurface = IMG_Load("assets/backgrounds and textures/sidebar.png");
    if (!sidebarSurface){
        std::cerr << "Failed to load sidebar PNG file: " << IMG_GetError() << "\n";
    }
    SDL_Surface* scoreSurface = IMG_Load("assets/backgrounds and textures/cloud.png");
    if (!scoreSurface)
        std::cerr << "Failed to load score PNG file: " << IMG_GetError() << "\n";

    SDL_Surface* cloudSurface = IMG_Load("assets/backgrounds and textures/cloud.png");
    if (!cloudSurface)
        std::cerr << "Failed to load cloud PNG file: " << IMG_GetError() << "\n";

    SDL_Surface* blockerSurface = IMG_Load("assets/backgrounds and textures/blocker.png");
    if (!blockerSurface)
        std::cerr << "Failed to load blocker image: " << IMG_GetError() << "\n";

    SDL_Surface* musicbarSurface = IMG_Load("assets/backgrounds and textures/musicbar.png");
    if (!musicbarSurface)
        std::cerr << "Failed to load music bar PNG file: " << IMG_GetError() << "\n";

    SDL_Surface* musictoggleSurface = IMG_Load("assets/backgrounds and textures/appletoggle.png");
    if (!musictoggleSurface)
        std::cerr << "Failed to load music toggle PNG file: " << IMG_GetError() << "\n";

    recordBackground = IMG_LoadTexture(renderer, "assets/backgrounds and textures/newrec.jpg");
    if (!recordBackground)
        std::cerr << "Failed to load record background: " << IMG_GetError() << "\n";

    gridBackground = IMG_LoadTexture(renderer, "assets/backgrounds and textures/gamegridbg.jpg");
    if (!gridBackground)
        std::cerr << "Failed to load grid background: " << IMG_GetError() << "\n";

    optionBackground = IMG_LoadTexture(renderer, "assets/backgrounds and textures/optionsbg.jpg");
    if (!optionBackground)
        std::cerr << "Failed to load option background: " << IMG_GetError() << "\n";

    gameoverBackground = IMG_LoadTexture(renderer, "assets/backgrounds and textures/optionsbg.jpg");
    if (!gameoverBackground)
        std::cerr << "Failed to load game over background: " << IMG_GetError() << "\n";

    gamewonBackground = IMG_LoadTexture(renderer, "assets/backgrounds and textures/optionsbg.jpg");
    if (!gamewonBackground)
        std::cerr << "Failed to load game won background: " << IMG_GetError() << "\n";

    startBackground = SDL_CreateTextureFromSurface(renderer, startSurface);
    SDL_FreeSurface(startSurface);
    if (!startBackground)
        std::cerr << "Failed to create start background texture: " << SDL_GetError() << "\n";

    sidebarBackground = SDL_CreateTextureFromSurface(renderer, sidebarSurface);
    SDL_FreeSurface(sidebarSurface);
    if (!sidebarBackground)
        std::cerr << "Failed to create sidebar background texture: " << SDL_GetError() << "\n";

    scoreBackground = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_FreeSurface(scoreSurface);
    if (!scoreBackground)
        std::cerr << "Failed to load score background: " << SDL_GetError() << "\n";

    blockerTexture = SDL_CreateTextureFromSurface(renderer, blockerSurface);
    if (!blockerSurface)
        std::cerr << "Failed to load blocker background: " << SDL_GetError() << "\n";

    cloudTexture = SDL_CreateTextureFromSurface(renderer, cloudSurface);
    SDL_FreeSurface(cloudSurface);
    if (!cloudTexture)
        std::cerr << "Failed to create cloud texture: " << SDL_GetError() << "\n";

    musicbarTexture = SDL_CreateTextureFromSurface(renderer, musicbarSurface);
    SDL_FreeSurface(musicbarSurface);
    if (!musicbarTexture)
        std::cerr << "Failed to create music bar texture: " << SDL_GetError() << "\n";

    musictoggleTexture = SDL_CreateTextureFromSurface(renderer, musictoggleSurface);
    SDL_FreeSurface(musictoggleSurface);
    if (!musictoggleTexture)
        std::cerr << "Failed to create music toggle texture: " << SDL_GetError() << "\n";

    std::string fruitNames[] = {
        "apple", "banana", "dragonfruit", "grape", "mango",
        "orange", "peach", "pineapple", "pomegranate", "strawberry", "watermelon"
    };
    int fruitValues[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};

    for (int i = 0; i < 11; i++) {
        std::string path = "assets/Fruit/" + fruitNames[i] + ".jpg";
        SDL_Surface* surf = IMG_Load(path.c_str());
        if (!surf) {
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << "\n";
            continue;
        }
        fruitTextures[fruitValues[i]] = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }

    for (int i = 0; i < 5; i++){
        std::string path = "assets/backgrounds and textures/gameover/gameoverbg" + std::to_string(i+1) + ".png";
        SDL_Surface* surf = IMG_Load(path.c_str());
        if (!surf){
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << "\n";
            continue;
        }
        gameoverTextures[i+1] = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }

    for (int i = 0; i < 5; i++){
        std::string path = "assets/backgrounds and textures/gamewin/winbg" + std::to_string(i+1) + ".png";
        SDL_Surface* surf = IMG_Load(path.c_str());
        if (!surf){
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << "\n";
            continue;
        }
        gamewinTextures[i+1] = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
    return true;
}

void freeAllTextures()
{
    if (startBackground)    { SDL_DestroyTexture(startBackground);    startBackground = nullptr; }
    if (gridBackground)     { SDL_DestroyTexture(gridBackground);     gridBackground = nullptr; }
    if (sidebarBackground)  { SDL_DestroyTexture(sidebarBackground);  sidebarBackground = nullptr;}
    if (scoreBackground)    { SDL_DestroyTexture(scoreBackground);    scoreBackground = nullptr;}
    if (recordBackground)   { SDL_DestroyTexture(recordBackground);   recordBackground = nullptr;}
    if (optionBackground)   { SDL_DestroyTexture(optionBackground);   optionBackground = nullptr; }
    if (gameoverBackground) { SDL_DestroyTexture(gameoverBackground); gameoverBackground = nullptr;}
    if (gamewonBackground)  { SDL_DestroyTexture(gamewonBackground);  gamewonBackground = nullptr;}
    if (blockerTexture)     { SDL_DestroyTexture(blockerTexture);     blockerTexture = nullptr;}
    if (cloudTexture)       { SDL_DestroyTexture(cloudTexture);       cloudTexture = nullptr; }
    if (musicbarTexture)    { SDL_DestroyTexture(musicbarTexture);    musicbarTexture = nullptr; }
    if (musictoggleTexture) { SDL_DestroyTexture(musictoggleTexture); musictoggleTexture = nullptr; }

    for (auto& kv : fruitTextures) {
        if (kv.second) {
            SDL_DestroyTexture(kv.second);
        }
    }
    fruitTextures.clear();

    for (auto& kv : gameoverTextures) {
        if (kv.second){
            SDL_DestroyTexture(kv.second);
        }
    }
    gameoverTextures.clear();

    for (auto& kv : gamewinTextures) {
        if (kv.second){
            SDL_DestroyTexture(kv.second);
        }
    }
    gamewinTextures.clear();
}

void cleanupTextures() {
    SDL_DestroyTexture(gridBackground);
    SDL_DestroyTexture(optionBackground);
    SDL_DestroyTexture(startBackground);
    SDL_DestroyTexture(sidebarBackground);
    SDL_DestroyTexture(recordBackground);
    SDL_DestroyTexture(gameoverBackground);
    SDL_DestroyTexture(scoreBackground);
    SDL_DestroyTexture(blockerTexture);
    SDL_DestroyTexture(cloudTexture);
    SDL_DestroyTexture(musicbarTexture);
    SDL_DestroyTexture(musictoggleTexture);
}
