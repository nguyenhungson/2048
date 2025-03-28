#ifndef AUDIO_H
#define AUDIO_H

#include <SDL_mixer.h>

bool initAudio();

void MusicFinishedCallback();

/** Free all music and sound effect resources, close audio. */
void cleanupAudio();

/** Play the background music. */
void playBackgroundMusic();

/** Stop any playing music. */
void stopMusic();

/** Play the game-win music in a loop. */
void playGameWinMusic();

/** Play the “new record” (congrats) music once. */
void playCongratsMusic();

/** Play the swipe SFX once. */
void playSwipeSFX();

/** Play the game-over SFX once. */
void playGameOverSFX();

/** Update the music volume (0–100). */
void setMusicVolume(int volume);

/** Update the SFX volume (0–100). */
void setSFXVolume(int volume);

#endif // AUDIO_H
