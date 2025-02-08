#pragma once
#include <string>
#include <unordered_set>
#include "glm/glm.hpp"
#include "rapidjson/document.h"
#include "SceneDB.h"

using std::string, glm::ivec2, std::pair;

class Engine
{
public:
	Engine(rapidjson::Document& game_config);

	void GameLoop();
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
	ivec2 InvertVelocity(ivec2 velocity);
	string RenderMap();
	Actor* GetPlayer();
	std::vector<Actor>* GetActors();
	std::unordered_set<int>* GetScoreActors();
	std::unordered_map<uint64_t, int>* GetBlockingPositionsToNum();
private:
	bool is_running = true;
	bool game_over_bad = false;
	bool game_over_good = false;
	int player_health = 3;
	int score = 0;
	int camera_width = 13;
	int camera_height = 9;
	string user_input = "";
	string game_start_message = "";
	string game_over_bad_message = "";
	string game_over_good_message = "";
	ivec2 new_position = ivec2(0, 0);
	SceneDB scene;
};