#pragma once
#include <string>
#include "glm/glm.hpp"
#include "MapHelper.h"

using std::string, glm::ivec2;

class Engine
{
public:
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
	string RenderMap();
private:
	bool is_running = true;
	int player_health = 3;
	int score = 0;
	string user_input = "";
	ivec2 player_position = ivec2(19, 15);
	ivec2 new_position = ivec2(0, 0);
};