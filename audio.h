#ifndef AUDIO_H
#define AUDIO_H
#include <SDL_mixer.h>

bool initAudio();

void MusicFinishedCallback();

void cleanupAudio();

void playBackgroundMusic();

void stopMusic();

void playGameWinMusic();

void playCongratsMusic();

void playSwipeSFX();

void playGameOverSFX();

void setMusicVolume(int volume);

void setSFXVolume(int volume);

#endif // AUDIO_H
