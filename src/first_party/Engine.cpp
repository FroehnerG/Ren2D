#include <iostream>
#include <string>
#include "Engine.h"

using std::string, std::cin, std::cout;

const string prompt = "Please make a decision...\n";
const string options = "Your options are \"n\", \"e\", \"s\", and \"w\", \"quit\"\n";

void Engine::GameLoop()
{
	cout << game_start_message << '\n';

	FirstRender();

	while (is_running) {
		Update();
		Render();
	}
}

void Engine::Input()
{
	cin >> user_input;

	if (user_input == "quit") {
		cout << game_over_bad_message;
		is_running = false;
	}
	else {
		FirstRender();
	}
}

void Engine::Update()
{
}

void Engine::Render()
{
}

void Engine::FirstRender()
{
	cout << initial_render << '\n';

	ShowScoreAndHealth();

	cout << prompt;

	cout << options;

	Input();
}

void Engine::ShowScoreAndHealth()
{
	cout << "health : " << player_health << ", score: " << score << '\n';
}
