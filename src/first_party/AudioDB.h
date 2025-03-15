#pragma once
#include "SDL2/SDL.h"
#include "SDL2_mixer/SDL_mixer.h"
#include "rapidjson/document.h"
#include "AudioHelper.h"

class AudioDB {
public:
	void LoadAudio(rapidjson::Document& game_config, std::string audio_name, bool is_intro);
	void PlayMusic(bool is_intro);
	void HaltMusic();
	bool HasIntroMusic();
	bool HasGameplayMusic();
private:
	Mix_Chunk* intro_music = nullptr;
	Mix_Chunk* gameplay_music = nullptr;
	bool has_intro_music = true;
	bool has_gameplay_music = true;
};
