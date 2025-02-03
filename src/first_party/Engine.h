#pragma once
#include <string>
#include <unordered_set>
#include "glm/glm.hpp"
#include "MapHelper.h"
#include "rapidjson/document.h"

using std::string, glm::ivec2;

class Engine
{
public:
	Engine(rapidjson::Document& config);

	void GameLoop();
	void Input();
	void Update();
	void Render();
	void ShowScoreAndHealth();
	bool IsPositionValid(ivec2 position);
	bool IsNPCAdjacent(ivec2 NPC_position);
	bool IsNPCInSameCell(ivec2 NPC_position);
	bool IsBlockingActorAtPosition(ivec2 position);
	void ShowNPCDialogue();
	void CheckNPCDialogue(string dialogue, string NPC_name);
	void MoveNPCs();
	ivec2 InvertVelocity(ivec2 velocity);
	string RenderMap();
private:
	bool is_running = true;
	bool game_over_bad = false;
	bool game_over_good = false;
	int player_health = 3;
	int score = 0;
	string user_input = "";
	string game_start_message = "";
	string game_over_bad_message = "";
	string game_over_good_message = "";
	ivec2 new_position = ivec2(0, 0);
	std::unordered_set<string> score_actors;
};