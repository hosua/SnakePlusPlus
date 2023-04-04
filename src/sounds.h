#ifndef SOUNDS_H
#define SOUNDS_H

#include <iostream>
#include <assert.h>
#include <memory>
#include <array>

#include <SDL2/SDL.h> // SDL2 library
#include <SDL2/SDL_scancode.h> // For reading I/O device input
#include <SDL2/SDL_ttf.h> // For using fonts in SDL
#include <SDL2/SDL_mixer.h> // For sounds

#define NUM_SOUNDS 4
#define SND_PATH_EAT "assets/sounds/eat.wav"
#define SND_PATH_EXPLOSION "assets/sounds/explosion.wav"
#define SND_PATH_MENUHOVER "assets/sounds/menu_hover.wav"
#define SND_PATH_MENUSELECT "assets/sounds/menu_select.wav"

typedef enum {
	S_EAT,
	S_EXPLOSION,
	S_MENUHOVER,
	S_MENUSELECT,
} SoundType;

class SoundMaster {
public:
	SoundMaster();
	~SoundMaster();
	void toggleMuted(){ _muted = !_muted; } // Nice simple way to toggle a flag
	bool isMuted(){ return _muted; }	
	Mix_Chunk* getSound(SoundType sound_type);
	bool loadSounds();
private:
	bool _muted;
	std::array<Mix_Chunk*, NUM_SOUNDS> _bank; // Sound bank
};

bool initSounds();

#endif // SOUNDS_H
