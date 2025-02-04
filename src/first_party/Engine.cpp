#include <iostream>
#include <string>
#include <unordered_set>
#include "rapidjson/document.h"
#include "Engine.h"

using std::string, std::cin, std::cout;

const string prompt = "Please make a decision...\n";
const string options = "Your options are \"n\", \"e\", \"s\", \"w\", \"quit\"\n";

const ivec2 ADJACENT_OFFSETS[8] = {
    ivec2(0, -1), 
    ivec2(0, 1), 
    ivec2(-1, 0), 
    ivec2(1, 0), // Cardinal directions
    ivec2(-1, -1), 
    ivec2(-1, 1), 
    ivec2(1, -1), 
    ivec2(1, 1) // Diagonal directions
};

Engine::Engine(rapidjson::Document& game_config)
{
    if (game_config.HasMember("game_start_message")) {
        game_start_message = game_config["game_start_message"].GetString();
    }

    if (game_config.HasMember("game_over_good_message")) {
        game_over_good_message = game_config["game_over_good_message"].GetString();
    }

    if (game_config.HasMember("game_over_bad_message")) {
        game_over_bad_message = game_config["game_over_bad_message"].GetString();
    }
}

void Engine::GameLoop()
{
	cout << game_start_message << '\n';

	Render();

	while (is_running) {
		Input();

        if (!is_running) {
            return;
        }

		Update();
		Render();
	}
}

void Engine::Input()
{
    ShowScoreAndHealth();
    cout << prompt << options;

	cin >> user_input;

	if (user_input == "quit") {
		cout << game_over_bad_message;
		is_running = false;
        return;
	}
	else if (user_input == "n") {
        hardcoded_actors.back().velocity = ivec2(0, -1);
	}
	else if (user_input == "e") {
        hardcoded_actors.back().velocity = ivec2(1, 0);
	}
	else if (user_input == "s") {
        hardcoded_actors.back().velocity = ivec2(0, 1);
	}
    else if (user_input == "w") {
        hardcoded_actors.back().velocity = ivec2(-1, 0);
    }
    else {
        hardcoded_actors.back().velocity = ivec2(0, 0);
    }
}

bool Engine::IsBlockingActorAtPosition(ivec2 position)
{
    for (const auto& actor : hardcoded_actors) {
        if (actor.position == position && actor.blocking) {
            return true;
        }
    }

    return false;
}

bool Engine::IsPositionValid(ivec2 position)
{
    // If tile is a wall or if there is a blocking player at that location, return false
    if (hardcoded_map[position.y][position.x] == 'b' || IsBlockingActorAtPosition(position)) {
        return false;
    }

    return true;
}

void Engine::MoveNPCs()
{
    // Go through each actor in actor vector and move them
    for (auto& actor : hardcoded_actors) {
        if (actor.velocity != ivec2(0, 0)) {
            ivec2 new_npc_position = actor.position + actor.velocity;

            // If actor can move to new location, move them
            if (IsPositionValid(new_npc_position)) {
                actor.position = new_npc_position;

                if (actor.actor_name == "player") {
                    hardcoded_actors.back().position = new_npc_position;
                }
            }
            // If actor cannot move to new location and is not the player, invert velocity
            else if (actor.actor_name != "player") {
                actor.velocity = InvertVelocity(actor.velocity);
            }
        }
    }
}

ivec2 Engine::InvertVelocity(ivec2 velocity)
{
    return ivec2(velocity.x * -1, velocity.y * -1);
}

void Engine::Update()
{
    MoveNPCs();
}

void Engine::Render()
{
    cout << RenderMap();
    ShowNPCDialogue();
}

void Engine::InitResolution(rapidjson::Document& rendering_config)
{
    camera_width = rendering_config["x_resolution"].GetInt();
    camera_height = rendering_config["y_resolution"].GetInt();
}

void Engine::ShowScoreAndHealth()
{
	cout << "health : " << player_health << ", score : " << score << '\n';
}

bool Engine::IsNPCAdjacent(ivec2 NPC_position)
{
    // Check all neighbors
    for (const auto& offset : ADJACENT_OFFSETS) {
        ivec2 neighbor_position = hardcoded_actors.back().position + offset;

        if (NPC_position == neighbor_position) {
            return true; // Found an adjacent actor
        }
    }

    return false;
}

bool Engine::IsNPCInSameCell(ivec2 NPC_position)
{
    if (NPC_position == hardcoded_actors.back().position) {
        return true;
    }

    return false;
}

void Engine::ShowNPCDialogue()
{
    for (const auto& actor : hardcoded_actors) {
        if (IsNPCAdjacent(actor.position) && actor.nearby_dialogue != "") {
            cout << actor.nearby_dialogue << '\n';
            CheckNPCDialogue(actor.nearby_dialogue, actor.actor_name);
        }
        else if (IsNPCInSameCell(actor.position) && actor.contact_dialogue != "") {
            cout << actor.contact_dialogue << '\n';
            CheckNPCDialogue(actor.contact_dialogue, actor.actor_name);
        }
    }

    if (!is_running) {
        ShowScoreAndHealth();
        if (game_over_good) {
            cout << game_over_good_message;
            return;
        }

        cout << game_over_bad_message;
    }
}

void Engine::CheckNPCDialogue(string dialogue, string NPC_name)
{
    string health_down = "health down";
    string score_up = "score up";
    string you_win = "you win";
    string game_over = "game over";

    if (dialogue.find(health_down) != string::npos) {
        player_health--;

        if (player_health <= 0) {
            is_running = false;
            game_over_bad = true;
        }
    }
    else if (dialogue.find(score_up) != string::npos && score_actors.find(NPC_name) == score_actors.end()) {
        score++;
        score_actors.insert(NPC_name);
    }
    else if (dialogue.find(you_win) != string::npos) {
        is_running = false;
        game_over_good = true;
    }
    else if (dialogue.find(game_over) != string::npos) {
        is_running = false;
        game_over_bad = true;
    }
}

std::string Engine::RenderMap()
{
    // Compute camera bounds directly without clamping
    int camera_x = hardcoded_actors.back().position.x - camera_width / 2;
    int camera_y = hardcoded_actors.back().position.y - camera_height / 2;

    // Create a dynamically sized 2D vector for the visible map
    std::vector<std::string> visible(camera_height, std::string(camera_width, ' '));

    // Populate the visible map with the corresponding map section
    for (int y = 0; y < camera_height; ++y) {
        for (int x = 0; x < camera_width; ++x) {
            // Calculate the corresponding map position
            int map_x = camera_x + x;
            int map_y = camera_y + y;

            // If the position is within the map, use the map character; otherwise, keep it as ' '
            if (map_y >= 0 && map_y < HARDCODED_MAP_HEIGHT &&
                map_x >= 0 && map_x < HARDCODED_MAP_WIDTH) {
                visible[y][x] = hardcoded_map[map_y][map_x];
            }
        }
    }

    // Overlay actors onto the visible map
    for (const Actor& actor : hardcoded_actors) {
        int ax = actor.position.x;
        int ay = actor.position.y;

        // Check if the actor is within the camera's view
        if (ax >= camera_x && ax < camera_x + camera_width &&
            ay >= camera_y && ay < camera_y + camera_height) {
            int vx = ax - camera_x; // Actor's x position in the visible map
            int vy = ay - camera_y; // Actor's y position in the visible map
            visible[vy][vx] = actor.view;
        }
    }

    // Convert the visible map to a string
    std::string rendered_map;
    for (const std::string& row : visible) {
        rendered_map += row + '\n';
    }

    return rendered_map;
}

