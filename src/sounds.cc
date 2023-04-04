#include "sounds.h"

Mix_Chunk* SoundMaster::getSound(SoundType sound_type){
	int st = (int) sound_type;
	assert(0 <= st && st < NUM_SOUNDS && "Error: Invalid sound type, fix this now you idiot!\n");
	return _bank[sound_type];
}

SoundMaster::SoundMaster(): _muted(false) {
	for (Mix_Chunk*& mc : _bank)
		mc = nullptr;
	if (!loadSounds())
		std::cerr << "Warning! Sound file failed to load, you may not hear sounds during gameplay.\n";
}

SoundMaster::~SoundMaster(){
	for (Mix_Chunk*& mc : _bank){
		Mix_FreeChunk(mc);
		mc = nullptr;
	}
	Mix_Quit();
}

bool SoundMaster::loadSounds(){ // Returns false if any sounds failed to load
	_bank[S_EAT] = Mix_LoadWAV(SND_PATH_EAT);
	if (!_bank[S_EAT]){
		std::cerr << "Error: Failed to load \"" << SND_PATH_EAT << "\"\n";
		return false;
	}
	_bank[S_EXPLOSION] = Mix_LoadWAV(SND_PATH_EXPLOSION);
	if (!_bank[S_EXPLOSION]){
		std::cerr << "Error: Failed to load \"" << SND_PATH_EXPLOSION << "\"\n";
		return false;
	}
	_bank[S_MENUHOVER] = Mix_LoadWAV(SND_PATH_MENUHOVER);
	if (!_bank[S_MENUHOVER]){
		std::cerr << "Error: Failed to load \"" << SND_PATH_MENUHOVER << "\"\n";
		return false;
	}
	_bank[S_MENUSELECT] = Mix_LoadWAV(SND_PATH_MENUSELECT);
	if (!_bank[S_MENUSELECT]){
		std::cerr << "Error: Failed to load \"" << SND_PATH_MENUSELECT << "\"\n";
		return false;
	}

	return true;
}

bool initSounds(){
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		return false;
	return true;
}
