#include "globals.h"
#include "audio.h"
#include "boosters.h"
#include <iostream>
#include <SDL_mixer.h>

bool initAudio()
{
    // Boosters SFX
    hammerSound = Mix_LoadWAV("assets/music and sfx/thorhammer.wav");
    if (!hammerSound){
        std::cerr << "Failed to load hammer sound: " << Mix_GetError() << "\n";
    }

    freezeSound = Mix_LoadWAV("assets/music and sfx/freeze.wav");
    if (!freezeSound){
        std::cerr << "Failed to load freeze sound: " << Mix_GetError() << "\n";
    }

    tsunamiSound = Mix_LoadWAV("assets/music and sfx/tsunami.wav");
    if (!tsunamiSound){
        std::cerr << "Failed to load tsunami sound: " << Mix_GetError() << "\n";
    }

    bgMusic = Mix_LoadMUS("assets/music and sfx/linga guli guli.mp3");
    if (!bgMusic) {
        std::cerr << "Failed to load bgMusic: " << Mix_GetError() << "\n";
        return false;
    }
    else{
        MusicFinishedCallback();
    }
    gameWinMusic = Mix_LoadMUS("assets/music and sfx/congratulation.mp3");
    if (!gameWinMusic) {
        std::cerr << "Failed to load gameWinMusic: " << Mix_GetError() << "\n";
    }
    congratsMusic = Mix_LoadMUS("assets/music and sfx/newrec.mp3");
    if (!congratsMusic) {
        std::cerr << "Failed to load congratsMusic: " << Mix_GetError() << "\n";
    }
    swipeSound = Mix_LoadWAV("assets/music and sfx/switch.wav");
    if (!swipeSound) {
        std::cerr << "Failed to load swipeSound: " << Mix_GetError() << "\n";
    }
    gameOverSound = Mix_LoadWAV("assets/music and sfx/gameover.wav");
    if (!gameOverSound) {
        std::cerr << "Failed to load gameOverSound: " << Mix_GetError() << "\n";
    }

    setMusicVolume(musicVolume);
    setSFXVolume(sfxVolume);

    return true;
}

void cleanupAudio()
{
    if (hammerSound)     { Mix_FreeChunk(hammerSound);     hammerSound = nullptr; }
    if (freezeSound)     { Mix_FreeChunk(freezeSound);     freezeSound = nullptr; }
    if (tsunamiSound)    { Mix_FreeChunk(tsunamiSound);    tsunamiSound = nullptr; }

    if (bgMusic)         { Mix_FreeMusic(bgMusic);         bgMusic = nullptr; }
    if (gameWinMusic)    { Mix_FreeMusic(gameWinMusic);    gameWinMusic = nullptr; }
    if (congratsMusic)   { Mix_FreeMusic(congratsMusic);   congratsMusic = nullptr; }

    if (swipeSound)      { Mix_FreeChunk(swipeSound);      swipeSound = nullptr; }
    if (gameOverSound)   { Mix_FreeChunk(gameOverSound);   gameOverSound = nullptr; }
}

void MusicFinishedCallback() {
    if (bgMusic) {
        Mix_PlayMusic(bgMusic, -1);
    }
}

void stopMusic()
{
    Mix_HaltMusic();
}

void playGameWinMusic()
{
    if (gameWinMusic) {
        Mix_PlayMusic(gameWinMusic, -1); // or loop once if you prefer
    }
}

void playCongratsMusic()
{
    if (congratsMusic) {
        Mix_PlayMusic(congratsMusic, 1); // play once
    }
}

void playSwipeSFX()
{
    if (swipeSound) {
        Mix_PlayChannel(-1, swipeSound, 0);
    }
}

void playGameOverSFX()
{
    if (gameOverSound) {
        Mix_PlayChannel(-1, gameOverSound, 0);
    }
}

void setMusicVolume(int volume)
{
    musicVolume = volume;
    int sdlVolume = (volume * 128) / 100;
    Mix_VolumeMusic(sdlVolume);
}

void setSFXVolume(int volume)
{
    sfxVolume = volume;
    int sdlVolume = (volume * 128) / 100;
    if (swipeSound)    Mix_VolumeChunk(swipeSound, sdlVolume);
    if (gameOverSound) Mix_VolumeChunk(gameOverSound, sdlVolume);
}
