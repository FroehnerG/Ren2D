#pragma once
#include <string>
#include <unordered_set>
#include "glm/glm.hpp"
#include "rapidjson/document.h"
#include "SceneDB.h"
#include "Renderer.h"
#include "ImageDB.h"
#include "TextDB.h"
#include "AudioDB.h"

using std::string, glm::ivec2, std::pair;

class Engine
{
public:
	Engine(rapidjson::Document& game_config);

	void GameLoop();
	void PlayIntro();
	void Input();
	void Update();
	void Render();
	void InitResolution(rapidjson::Document& rendering_config);
	void ShowScoreAndHealth();
	bool IsPositionValid(ivec2 position);
	bool IsNPCAdjacent(ivec2 NPC_position);
	bool IsNPCInSameCell(ivec2 NPC_position);
	void ShowNPCDialogue();
	void CheckNPCDialogue(string dialogue, int actor_id);
	void MoveNPCs();
	void LoadScene(string scene_name);
	SDL_Window* CreateWindow();
	ivec2 InvertVelocity(ivec2 velocity);
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
	int player_health = 3;
	int score = 0;
	int x_resolution = 640;
	int y_resolution = 360;
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