#include "textures.h"
#include "globals.h"
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <map>

SDL_Texture* startBackground    = nullptr;
SDL_Texture* gridBackground     = nullptr;
SDL_Texture* optionBackground   = nullptr;
SDL_Texture* cloudTexture       = nullptr;
SDL_Texture* musicbarTexture    = nullptr;
SDL_Texture* musictoggleTexture = nullptr;
std::map<int, SDL_Texture*> fruitTextures;

bool loadAllTextures(SDL_Renderer* renderer)
{
    SDL_Surface* startSurface = IMG_Load("backgrounds and textures/startbg.png");
    if (!startSurface)
        std::cerr << "Failed to load start PNG file: " << IMG_GetError() << "\n";

    SDL_Surface* cloudSurface = IMG_Load("backgrounds and textures/cloud.png");
    if (!cloudSurface)
        std::cerr << "Failed to load cloud PNG file: " << IMG_GetError() << "\n";

    SDL_Surface* musicbarSurface = IMG_Load("backgrounds and textures/musicbar.png");
    if (!musicbarSurface)
        std::cerr << "Failed to load music bar PNG file: " << IMG_GetError() << "\n";

    SDL_Surface* musictoggleSurface = IMG_Load("backgrounds and textures/appletoggle.png");
    if (!musictoggleSurface)
        std::cerr << "Failed to load music toggle PNG file: " << IMG_GetError() << "\n";

    gridBackground = IMG_LoadTexture(renderer, "backgrounds and textures/gamegridbg.jpg");
    if (!gridBackground)
        std::cerr << "Failed to load grid background: " << IMG_GetError() << "\n";

    optionBackground = IMG_LoadTexture(renderer, "backgrounds and textures/optionsbg.jpg");
    if (!optionBackground)
        std::cerr << "Failed to load option background: " << IMG_GetError() << "\n";

    startBackground = SDL_CreateTextureFromSurface(renderer, startSurface);
    SDL_FreeSurface(startSurface);
    if (!startBackground)
        std::cerr << "Failed to create start background texture: " << SDL_GetError() << "\n";

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
    // Load other backgrounds similarly...
    // load fruit images:
    std::string fruitNames[] = {
        "apple", "banana", "dragonfruit", "grape", "mango",
        "orange", "peach", "pineapple", "pomegranate", "strawberry", "watermelon"
    };
    int values[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};

    for (int i = 0; i < 11; i++) {
        std::string path = "Fruit/" + fruitNames[i] + ".jpg";
        SDL_Surface* surf = IMG_Load(path.c_str());
        if (!surf) {
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << "\n";
            continue;
        }
        fruitTextures[values[i]] = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
    // etc.
    return true; // or check if everything loaded
}

void freeAllTextures()
{
    if (startBackground)    { SDL_DestroyTexture(startBackground);    startBackground = nullptr; }
    if (gridBackground)     { SDL_DestroyTexture(gridBackground);     gridBackground = nullptr; }
    if (optionBackground)   { SDL_DestroyTexture(optionBackground);   optionBackground = nullptr; }
    if (cloudTexture)       { SDL_DestroyTexture(cloudTexture);       cloudTexture = nullptr; }
    if (musicbarTexture)    { SDL_DestroyTexture(musicbarTexture);    musicbarTexture = nullptr; }
    if (musictoggleTexture) { SDL_DestroyTexture(musictoggleTexture); musictoggleTexture = nullptr; }

    for (auto& kv : fruitTextures) {
        if (kv.second) {
            SDL_DestroyTexture(kv.second);
        }
    }
    fruitTextures.clear();
}

void cleanupTextures() {
    SDL_DestroyTexture(gridBackground);
    SDL_DestroyTexture(optionBackground);
    SDL_DestroyTexture(startBackground);
    SDL_DestroyTexture(cloudTexture);
    SDL_DestroyTexture(musicbarTexture);
    SDL_DestroyTexture(musictoggleTexture);
}
