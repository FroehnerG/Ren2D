#pragma once
#include <unordered_map>
#include <utility>
#include "SDL2/SDL.h"
#include "SDL2_mixer/SDL_mixer.h"
#include "rapidjson/document.h"
#include "AudioHelper.h"

class AudioDB {
public:
	AudioDB();
	void LoadAudio(rapidjson::Document& game_config, std::string audio_name, bool is_intro);
	void PlayMusic(bool is_intro);
	void PlayGameOverMusic(bool is_good);
	void SetNearbyDialogueSFXByID(int actor_id, std::string sfx_name);
	std::pair<Mix_Chunk*, bool> GetNearbyDialogueSFXByID(int actor_id);
	void LoadPlayerSFX(std::string sfx_type, std::string sfx_name);
	void PlayActorSFX(int actor_id, std::string sfx_name, int channel);
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
	std::unordered_map<int, std::pair<Mix_Chunk*, bool>> nearby_dialogue_sfx_by_id;
	Mix_Chunk* intro_music = nullptr;
	Mix_Chunk* gameplay_music = nullptr;
	Mix_Chunk* game_over_good_audio = nullptr;
	Mix_Chunk* game_over_bad_audio = nullptr;
	Mix_Chunk* step_sfx = nullptr;
	Mix_Chunk* score_sfx = nullptr;
	Mix_Chunk* damage_sfx = nullptr;
	bool has_step_sfx = false;
	bool has_score_sfx = false;
	bool has_damage_sfx = false;
	bool has_intro_music = true;
	bool has_gameplay_music = true;
};
