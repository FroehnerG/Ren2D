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
	void FirstRender();
	void ShowScoreAndHealth();
private:
	bool is_running = true;
	int player_health = 3;
	int score = 0;
	string user_input = "";
	ivec2 player_position = ivec2(0, 0);
};