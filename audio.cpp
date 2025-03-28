#include "globals.h"
#include "audio.h"
#include <iostream> // For std::cerr
#include <SDL_mixer.h>

// Globals
int musicVolume = 100;
int sfxVolume   = 100;

Mix_Music* bgMusic       = nullptr;
Mix_Music* gameWinMusic  = nullptr;
Mix_Music* congratsMusic = nullptr;

Mix_Chunk* swipeSound    = nullptr;
Mix_Chunk* gameOverSound = nullptr;

bool initAudio()
{
    // Initialize SDL_mixer if not done yet by main (optional).
    // Typically, you might do Mix_OpenAudio(...) in main, but you could do it here.

    // Load your music and SFX files.
    bgMusic = Mix_LoadMUS("music and sfx/linga guli guli.mp3");
    if (!bgMusic) {
        std::cerr << "Failed to load bgMusic: " << Mix_GetError() << "\n";
        return false;
    }
    gameWinMusic = Mix_LoadMUS("music and sfx/congratulation.mp3");
    if (!gameWinMusic) {
        std::cerr << "Failed to load gameWinMusic: " << Mix_GetError() << "\n";
        // Not returning false here in case you want partial fallback.
    }
    congratsMusic = Mix_LoadMUS("music and sfx/newrec.mp3");
    if (!congratsMusic) {
        std::cerr << "Failed to load congratsMusic: " << Mix_GetError() << "\n";
    }
    swipeSound = Mix_LoadWAV("music and sfx/switch.wav");
    if (!swipeSound) {
        std::cerr << "Failed to load swipeSound: " << Mix_GetError() << "\n";
    }
    gameOverSound = Mix_LoadWAV("music and sfx/gameover.wav");
    if (!gameOverSound) {
        std::cerr << "Failed to load gameOverSound: " << Mix_GetError() << "\n";
    }

    // Set default volumes.
    setMusicVolume(musicVolume);
    setSFXVolume(sfxVolume);

    return true; // Indicate success (with partial loads if some are missing).
}

void cleanupAudio()
{
    if (bgMusic)         { Mix_FreeMusic(bgMusic);         bgMusic = nullptr; }
    if (gameWinMusic)    { Mix_FreeMusic(gameWinMusic);    gameWinMusic = nullptr; }
    if (congratsMusic)   { Mix_FreeMusic(congratsMusic);   congratsMusic = nullptr; }

    if (swipeSound)      { Mix_FreeChunk(swipeSound);      swipeSound = nullptr; }
    if (gameOverSound)   { Mix_FreeChunk(gameOverSound);   gameOverSound = nullptr; }

    // Typically Mix_CloseAudio() is called in main cleanup if needed.
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
    // SDL_mixer range for Mix_VolumeMusic is 0–128.
    int sdlVolume = (volume * 128) / 100;
    Mix_VolumeMusic(sdlVolume);
}

void setSFXVolume(int volume)
{
    sfxVolume = volume;
    // Also 0–128 for chunk volume.
    int sdlVolume = (volume * 128) / 100;
    if (swipeSound)    Mix_VolumeChunk(swipeSound, sdlVolume);
    if (gameOverSound) Mix_VolumeChunk(gameOverSound, sdlVolume);
}
