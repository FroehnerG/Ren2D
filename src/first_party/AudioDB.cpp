#include <string>
#include <iostream>
#include <filesystem>
#include <cstdlib> // For exit(0)
#include "AudioDB.h"

namespace fs = std::filesystem;

AudioDB::AudioDB()
{
	AudioHelper::Mix_AllocateChannels(50);
}

void AudioDB::LoadAudio(rapidjson::Document& game_config, std::string audio_type, bool is_intro)
{
	if (is_intro && !game_config.HasMember(audio_type.c_str())) {
		has_intro_music = false;
		return;
	}
	
	if (audio_type == "gameplay_audio" && !game_config.HasMember("gameplay_audio")) {
		has_gameplay_music = false;
		return;
	}

	if (audio_type == "game_over_good_audio" && !game_config.HasMember("game_over_good_audio")) {
		has_game_over_good_audio = false;
		return;
	}

	if (audio_type == "game_over_bad_audio" && !game_config.HasMember("game_over_bad_audio")) {
		has_game_over_bad_audio = false;
		return;
	}

	if (audio_type == "score_sfx" && !game_config.HasMember("score_sfx")) {
		return;
	}

	std::string audio_name = game_config[audio_type.c_str()].GetString();

	std::string audio_path_wav = "resources/audio/" + audio_name + ".wav";
	std::string audio_path_ogg = "resources/audio/" + audio_name + ".ogg";

	if (!fs::exists(audio_path_wav) && !fs::exists(audio_path_ogg)) {
		std::cout << "error: failed to play audio clip " << audio_name;
		exit(0);
	}

	if (AudioHelper::Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0) {
		exit(0);
	}

	if (is_intro) {
		if (fs::exists(audio_path_wav)) {
			intro_music = AudioHelper::Mix_LoadWAV(audio_path_wav.c_str());
		}
		else {
			intro_music = AudioHelper::Mix_LoadWAV(audio_path_ogg.c_str());
		}
	}
	else if (audio_type == "score_sfx") {
		if (fs::exists(audio_path_wav)) {
			score_sfx = AudioHelper::Mix_LoadWAV(audio_path_wav.c_str());
		}
		else {
			score_sfx = AudioHelper::Mix_LoadWAV(audio_path_ogg.c_str());
		}
		has_score_sfx = true;
	}
	else if (audio_type == "game_over_good_audio") {
		if (fs::exists(audio_path_wav)) {
			game_over_good_audio = AudioHelper::Mix_LoadWAV(audio_path_wav.c_str());
		}
		else {
			game_over_good_audio = AudioHelper::Mix_LoadWAV(audio_path_ogg.c_str());
		}
	}
	else if (audio_type == "game_over_bad_audio") {
		if (fs::exists(audio_path_wav)) {
			game_over_bad_audio = AudioHelper::Mix_LoadWAV(audio_path_wav.c_str());
		}
		else {
			game_over_bad_audio = AudioHelper::Mix_LoadWAV(audio_path_ogg.c_str());
		}
	}
	else {
		if (fs::exists(audio_path_wav)) {
			gameplay_music = AudioHelper::Mix_LoadWAV(audio_path_wav.c_str());
		}
		else {
			gameplay_music = AudioHelper::Mix_LoadWAV(audio_path_ogg.c_str());
		}
	}

	return;
}

__declspec(noinline) void AudioDB::PlayMusic(bool is_intro)
{
	volatile int dummy = 0;
	dummy++;

	if (!CheckIfHasMusic(is_intro)) {
		return;
	}

	if (is_intro && AudioHelper::Mix_PlayChannel(0, intro_music, -1) < 0) {
		exit(0);
	}
	else if (is_intro) {
		return;
	}

	if (AudioHelper::Mix_PlayChannel(0, gameplay_music, -1) < 0) {
		exit(0);
	}
}

void AudioDB::PlayGameOverMusic(bool is_good)
{
	if (is_good) {
		if (AudioHelper::Mix_PlayChannel(0, game_over_good_audio, 0) < 0) {
			exit(0);
		}
		return;
	}

	if (AudioHelper::Mix_PlayChannel(0, game_over_bad_audio, 0) < 0) {
		exit(0);
	}
	
}

void AudioDB::SetNearbyDialogueSFXByID(int actor_id, std::string sfx_name)
{
	std::string sfx_path_wav = "resources/audio/" + sfx_name + ".wav";
	std::string sfx_path_ogg = "resources/audio/" + sfx_name + ".ogg";

	if (fs::exists(sfx_path_wav)) {
		nearby_dialogue_sfx_by_id[actor_id].first = AudioHelper::Mix_LoadWAV(sfx_path_wav.c_str());
		nearby_dialogue_sfx_by_id[actor_id].second = false;
	}
	else {
		nearby_dialogue_sfx_by_id[actor_id].first = AudioHelper::Mix_LoadWAV(sfx_path_ogg.c_str());
		nearby_dialogue_sfx_by_id[actor_id].second = false;
	}
}

std::pair<Mix_Chunk*, bool> AudioDB::GetNearbyDialogueSFXByID(int actor_id)
{
	return nearby_dialogue_sfx_by_id[actor_id];
}

void AudioDB::LoadPlayerSFX(std::string sfx_type, std::string sfx_name)
{
	std::string sfx_path_wav = "resources/audio/" + sfx_name + ".wav";
	std::string sfx_path_ogg = "resources/audio/" + sfx_name + ".ogg";

	if (sfx_type == "step_sfx") {
		if (fs::exists(sfx_path_wav)) {
			step_sfx = AudioHelper::Mix_LoadWAV(sfx_path_wav.c_str());
		}
		else {
			step_sfx = AudioHelper::Mix_LoadWAV(sfx_path_ogg.c_str());
		}

		has_step_sfx = true;
	}
	else if (sfx_type == "damage_sfx") {
		if (fs::exists(sfx_path_wav)) {
			damage_sfx = AudioHelper::Mix_LoadWAV(sfx_path_wav.c_str());
		}
		else {
			damage_sfx = AudioHelper::Mix_LoadWAV(sfx_path_ogg.c_str());
		}

		has_damage_sfx = true;
	}
}

void AudioDB::PlayActorSFX(int actor_id, std::string sfx_name, int channel)
{
	if (sfx_name == "score_sfx") {
		if (has_score_sfx) {
			AudioHelper::Mix_PlayChannel(channel, score_sfx, 0);
		}
	}
	else if (sfx_name == "step_sfx") {
		if (has_step_sfx) {
			AudioHelper::Mix_PlayChannel(channel, step_sfx, 0);
		}
	}
	else if (sfx_name == "damage_sfx") {
		if (has_damage_sfx) {
			AudioHelper::Mix_PlayChannel(channel, damage_sfx, 0);
		}
	}
	else {
		if (nearby_dialogue_sfx_by_id.find(actor_id) == nearby_dialogue_sfx_by_id.end()) {
			return;
		}

		if (!nearby_dialogue_sfx_by_id[actor_id].second) {
			AudioHelper::Mix_PlayChannel(channel, nearby_dialogue_sfx_by_id[actor_id].first, 0);
			nearby_dialogue_sfx_by_id[actor_id].second = true;
		}
	}
}

bool AudioDB::CheckIfHasMusic(bool is_intro) {
	if (!has_intro_music && !has_gameplay_music) {
		return false;
	}

	if (is_intro && !has_intro_music) {
		return false;
	}

	if (!is_intro && !has_gameplay_music) {
		return false;
	}
}

void AudioDB::HaltMusic()
{
	if (!CheckIfHasMusic(true || false)) {
		return;
	}

	if (AudioHelper::Mix_HaltChannel(0) < 0) {
		exit(0);
	}
}

bool AudioDB::HasIntroMusic()
{
	return has_intro_music;
}

bool AudioDB::HasGameplayMusic()
{
	return has_gameplay_music;
}
