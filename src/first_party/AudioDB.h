#pragma once
#include "SDL2/SDL.h"
#include "SDL2_mixer/SDL_mixer.h"
#include "rapidjson/document.h"
#include "AudioHelper.h"

class AudioDB {
public:
	void LoadAudio(rapidjson::Document& game_config, std::string audio_name, bool is_intro);
	void PlayMusic(bool is_intro);
	void PlayGameOverMusic(bool is_good);
	bool CheckIfHasMusic(bool is_intro);
	void HaltMusic();
	bool HasIntroMusic();
	bool HasGameplayMusic();

	bool intro_music_playing = true;
	bool gameplay_music_playing = false;
	bool game_over_music_playing = false;

	bool has_game_over_good_audio = true;
	bool has_game_over_bad_audio = true;
private:
	Mix_Chunk* intro_music = nullptr;
	Mix_Chunk* gameplay_music = nullptr;
	Mix_Chunk* game_over_good_audio = nullptr;
	Mix_Chunk* game_over_bad_audio = nullptr;
	bool has_intro_music = true;
	bool has_gameplay_music = true;
};
