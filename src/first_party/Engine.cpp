#include <iostream>
#include <string>
#include "Engine.h"

using std::string, std::cin, std::cout;

const string prompt = "Please make a decision...\n";
const string options = "Your options are \"n\", \"e\", \"s\", \"w\", \"quit\"\n";
const int CAMERA_WIDTH = 13;
const int CAMERA_HEIGHT = 9;

void Engine::GameLoop()
{
	cout << game_start_message << '\n';

	Render();

	while (is_running) {
		Input();
		Update();
		Render();
	}
}

void Engine::Input()
{
    cout << prompt << options;

	cin >> user_input;

	if (user_input == "quit") {
		cout << game_over_bad_message;
		is_running = false;
	}
	else if (user_input == "n") {
		hardcoded_actors.back().position += ivec2(0, -1);
	}
	else if (user_input == "e") {
		hardcoded_actors.back().position += ivec2(1, 0);
	}
	else if (user_input == "s") {
		hardcoded_actors.back().position += ivec2(0, 1);
	}
	else if (user_input == "w") {
		hardcoded_actors.back().position += ivec2(-1, 0);
	}

    player_position = hardcoded_actors.back().position;
}

void Engine::Update()
{
}

void Engine::Render()
{
    cout << RenderMap();
    ShowScoreAndHealth();
}

void Engine::ShowScoreAndHealth()
{
	cout << "health : " << player_health << ", score : " << score << '\n';
}

bool Engine::IsPositionValid()
{

    return false;
}

string Engine::RenderMap()
{
    // Compute camera bounds directly without clamping
    int camera_x = player_position.x - CAMERA_WIDTH / 2;
    int camera_y = player_position.y - CAMERA_HEIGHT / 2;

    // Create a 2D array for the visible map
    char visible[CAMERA_HEIGHT][CAMERA_WIDTH + 1];
    for (int y = 0; y < CAMERA_HEIGHT; ++y) {
        for (int x = 0; x < CAMERA_WIDTH; ++x) {
            // Calculate the corresponding map position
            int map_x = camera_x + x;
            int map_y = camera_y + y;

            // If the position is within the map, use the map character; otherwise, use a blank space
            if (map_y >= 0 && map_y < HARDCODED_MAP_HEIGHT &&
                map_x >= 0 && map_x < HARDCODED_MAP_WIDTH) {
                visible[y][x] = hardcoded_map[map_y][map_x];
            }
            else {
                visible[y][x] = ' '; // Out-of-bounds areas are empty
            }
        }
        visible[y][CAMERA_WIDTH] = '\0'; // Null-terminate each row
    }

    // Overlay actors onto the visible map
    for (const Actor& actor : hardcoded_actors) {
        int ax = actor.position.x;
        int ay = actor.position.y;

        // Check if the actor is within the camera's view
        if (ax >= camera_x && ax < camera_x + CAMERA_WIDTH &&
            ay >= camera_y && ay < camera_y + CAMERA_HEIGHT) {
            int vx = ax - camera_x; // Actor's x position in the visible map
            int vy = ay - camera_y; // Actor's y position in the visible map
            visible[vy][vx] = actor.view;
        }
    }

    // Convert the visible map to a string
    std::string rendered_map;
    for (int y = 0; y < CAMERA_HEIGHT; ++y) {
        rendered_map += visible[y];
        rendered_map += '\n';
    }

    return rendered_map;
}
