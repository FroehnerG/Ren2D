#pragma once
#include <string>
#include <unordered_set>
#include <optional>
#include "glm/glm.hpp"
#include "rapidjson/document.h"
#include "SceneDB.h"
#include "Renderer.h"
#include "ImageDB.h"
#include "TextDB.h"
#include "AudioDB.h"

using std::string, glm::vec2, std::pair;

class Engine
{
public:
	Engine(rapidjson::Document& game_config);

	void GameLoop();
	void PlayIntro();
	void HandlePlayerMovement();
	void Update();
	void Render(vector<string>* dialogue);
	void UpdatePlayerPosition(vec2 new_position);
	void InitResolution(rapidjson::Document& rendering_config);
	void ShowScoreAndHealth();
	bool IsPositionValid(vec2 position);
	bool IsNPCAdjacent(vec2 NPC_position);
	bool IsNPCInSameCell(vec2 NPC_position);
	void ShowNPCDialogue(vector<string>* dialogue);
	void CheckNPCDialogue(string& dialogue, int actor_id);
	void MoveNPCs();
	void LoadScene(string scene_name);
	SDL_Window* CreateWindow();
	ivec2 InvertVelocity(vec2 velocity);
	// RenderMap();
	Actor* GetPlayer();
	std::vector<Actor>* GetActors();
	std::unordered_set<int>* GetScoreActors();
	std::unordered_map<uint64_t, int>* GetBlockingPositionsToNum();
private:
	bool is_running = true;
	bool game_over_bad = false;
	bool game_over_good = false;
	bool next_scene = false;
	float player_movement_speed = 0.02f;
	int player_health = 3;
	int score = 0;
	int x_resolution = 640;
	int y_resolution = 360;
	int last_damage_frame = -999999;  // Large negative number to allow damage on first interaction
	int current_frame = 0;
	string game_title = "";
	string user_input = "";
	string game_start_message = "";
	string game_over_bad_message = "";
	string game_over_good_message = "";
	string next_scene_name = "";
	ivec2 new_position = ivec2(0, 0);
	SceneDB scene;
	SDL_Window* window = nullptr;
	Renderer renderer;
	ImageDB images;
	TextDB text;
	AudioDB audio;
};