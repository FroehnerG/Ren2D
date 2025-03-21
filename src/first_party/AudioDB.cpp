#include <string>
#include <iostream>
#include <filesystem>
#include <cstdlib> // For exit(0)
#include "AudioDB.h"

namespace fs = std::filesystem;

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

void AudioDB::PlayMusic(bool is_intro)
{
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
