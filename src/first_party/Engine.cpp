#include <iostream>
#include <filesystem>
#include "rapidjson/document.h"
#include "EngineUtils.h"
#include "Engine.h"

using std::string, std::cin, std::cout;
namespace fs = std::filesystem;

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

	if (game_config.HasMember("game_title")) {
		game_title = game_config["game_title"].GetString();
	}

	if (game_config.HasMember("initial_scene")) {
		string scene_name = game_config["initial_scene"].GetString();

		string scene_path = "resources/scenes/" + scene_name + ".scene";

		if (!fs::exists(scene_path)) {
			cout << "error: scene " + scene_name + " is missing";
			exit(0);
		}

		rapidjson::Document scene_json;
		EngineUtils::ReadJsonFile(scene_path, scene_json);

		scene.LoadActors(scene_json);
	}
	else {
		cout << "error: initial_scene unspecified";
		exit(0);
	}

	window = CreateWindow();
	renderer.SetRenderer(window);
	SDL_SetRenderDrawColor(renderer.GetRenderer(), renderer.GetColor("red"), renderer.GetColor("green"), renderer.GetColor("blue"), 255);
	SDL_RenderClear(renderer.GetRenderer());
	images.LoadImages(game_config, renderer.GetRenderer(), "intro_image");
}

void Engine::GameLoop()
{
	PlayIntro();

	if (game_start_message != "")
		cout << game_start_message << '\n';

	while (is_running) {
		SDL_Event e;
		while (Helper::SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				Helper::SDL_RenderPresent(renderer.GetRenderer());
				exit(0);
			}

			// Only process input if intro images are still being shown
			if (images.IsIntroPlaying()) {
				if (e.type == SDL_KEYDOWN) {
					if (e.key.keysym.scancode == SDL_SCANCODE_SPACE ||
						e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
						images.AdvanceIntro();
					}
				}

				if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
					images.AdvanceIntro();
				}
			}
		}

		Input();
		Update();
		renderer.Render();
	}
}

void Engine::PlayIntro()
{
	// **Intro Sequence Loop**
	while (images.IsIntroPlaying()) {
		SDL_Event e;
		while (Helper::SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				exit(0);
			}

			// Handle advancing intro images
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.scancode == SDL_SCANCODE_SPACE ||
					e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
					images.AdvanceIntro();
				}
			}

			if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
				images.AdvanceIntro();
			}
		}

		// **Render only intro image**
		SDL_RenderClear(renderer.GetRenderer());
		images.RenderIntroImage(renderer.GetRenderer());
		Helper::SDL_RenderPresent(renderer.GetRenderer());
	}
}

void Engine::Input()
{
	Actor* player = GetPlayer();

	if (player != nullptr) {
		ShowScoreAndHealth();
		cout << prompt << options;

		cin >> user_input;

		if (user_input == "quit") {
			if (game_over_bad_message != "")
				cout << game_over_bad_message;

			is_running = false;
			return;
		}
		else if (user_input == "n") {
			player->velocity = ivec2(0, -1);
		}
		else if (user_input == "e") {
			player->velocity = ivec2(1, 0);
		}
		else if (user_input == "s") {
			player->velocity = ivec2(0, 1);
		}
		else if (user_input == "w") {
			player->velocity = ivec2(-1, 0);
		}
		else {
			player->velocity = ivec2(0, 0);
		}
	}
}

bool Engine::IsPositionValid(ivec2 position)
{
	uint64_t composite_position = EngineUtils::CreateCompositeKey(position);

	auto* blocking_map = GetBlockingPositionsToNum(); // Store pointer in a local variable

	auto it = blocking_map->find(composite_position);

	if (it != blocking_map->end() && it->second > 0) {
		return false;
	}

	return true;
}

void Engine::MoveNPCs()
{
	vector<Actor>* actors = GetActors();
	// Go through each actor in actor vector and move them
	for (auto& actor : *actors) {
		if (actor.velocity != ivec2(0, 0)) {
			ivec2 new_actor_position = actor.position + actor.velocity;

			// If actor can move to new location, move them
			if (IsPositionValid(new_actor_position)) {
				if (actor.blocking) {
					uint64_t composite_position = EngineUtils::CreateCompositeKey(new_actor_position);
					uint64_t old_composite_position = EngineUtils::CreateCompositeKey(actor.position);

					// Add new position and remove old position to blocking map
					(*GetBlockingPositionsToNum())[composite_position]++;
					(*GetBlockingPositionsToNum())[old_composite_position]--;
				}

				actor.position = new_actor_position;
			}
			// If actor cannot move to new location and is not the player, invert velocity
			else if (actor.actor_name != "player") {
				actor.velocity = InvertVelocity(actor.velocity);
			}
		}
	}
}

void Engine::LoadScene(string scene_name)
{
	string scene_path = "resources/scenes/" + scene_name + ".scene";

	if (!fs::exists(scene_path)) {
		cout << "error: scene " + scene_name + " is missing";
		exit(0);
	}

	rapidjson::Document scene_json;
	EngineUtils::ReadJsonFile(scene_path, scene_json);

	scene.Reset();

	scene.LoadActors(scene_json);
}

SDL_Window* Engine::CreateWindow()
{
	std::vector<char> game_title_cstr(game_title.begin(), game_title.end());
	game_title_cstr.push_back('\0'); // Ensure null termination

	return Helper::SDL_CreateWindow(game_title_cstr.data(), 500, 500, x_resolution, y_resolution, SDL_WINDOW_SHOWN);
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
	if (rendering_config.HasMember("x_resolution")) {
		x_resolution = rendering_config["x_resolution"].GetInt();
	}

	if (rendering_config.HasMember("y_resolution")) {
		y_resolution = rendering_config["y_resolution"].GetInt();
	}

	if (rendering_config.HasMember("clear_color_r") &&
		rendering_config.HasMember("clear_color_g") &&
		rendering_config.HasMember("clear_color_b")) {
		int r = rendering_config["clear_color_r"].GetInt();
		int g = rendering_config["clear_color_g"].GetInt();
		int b = rendering_config["clear_color_b"].GetInt();

		renderer.SetClearColor(r, g, b);
	}
}

void Engine::ShowScoreAndHealth()
{
	cout << "health : " << player_health << ", score : " << score << '\n';
}

bool Engine::IsNPCAdjacent(ivec2 NPC_position)
{
	// Check all neighbors
	Actor* player = GetPlayer();

	for (const auto& offset : ADJACENT_OFFSETS) {
		ivec2 neighbor_position = player->position + offset;

		if (NPC_position == neighbor_position) {
			return true; // Found an adjacent actor
		}
	}

	return false;
}

bool Engine::IsNPCInSameCell(ivec2 NPC_position)
{
	if (NPC_position == GetPlayer()->position) {
		return true;
	}

	return false;
}

void Engine::ShowNPCDialogue()
{
	vector<Actor>* actors = GetActors();

	for (const auto& actor : *actors) {
		if (IsNPCAdjacent(actor.position) && actor.nearby_dialogue != "") {
			cout << actor.nearby_dialogue << '\n';
			CheckNPCDialogue(actor.nearby_dialogue, actor.id);
		}
		else if (IsNPCInSameCell(actor.position) && actor.contact_dialogue != "") {
			cout << actor.contact_dialogue << '\n';
			CheckNPCDialogue(actor.contact_dialogue, actor.id);
		}
	}

	if (!is_running) {
		ShowScoreAndHealth();
		if (game_over_good) {
			if (game_over_good_message != "")
				cout << game_over_good_message;
			return;
		}

		if (game_over_bad_message != "")
			cout << game_over_bad_message;
	}

	if (next_scene) {
		next_scene = false;
		ShowScoreAndHealth();
		LoadScene(next_scene_name);
		Render();
	}
}

void Engine::CheckNPCDialogue(string dialogue, int actor_id)
{
	std::unordered_set<int>*score_actors = GetScoreActors();

	string health_down = "health down";
	string score_up = "score up";
	string you_win = "you win";
	string game_over = "game over";
	string proceed_to = "proceed to";

	if (dialogue.find(health_down) != string::npos) {
		player_health--;

		if (player_health <= 0) {
			is_running = false;
			game_over_bad = true;
		}
	}
	else if (dialogue.find(score_up) != string::npos && score_actors->find(actor_id) == score_actors->end()) {
		score++;
		score_actors->insert(actor_id);
	}
	else if (dialogue.find(you_win) != string::npos) {
		is_running = false;
		game_over_good = true;
	}
	else if (dialogue.find(game_over) != string::npos) {
		is_running = false;
		game_over_bad = true;
	}
	else if (dialogue.find(proceed_to) != string::npos) {
		next_scene = true;
		next_scene_name = EngineUtils::ObtainWordAfterPhrase(dialogue, proceed_to);
	}
}

std::string Engine::RenderMap()
{
	// Compute camera bounds directly based on the player's position
	Actor* player = GetPlayer();
	
	int camera_x = 0;
	int camera_y = 0;

	if (player != nullptr) {
		int camera_x = player->position.x - x_resolution / 2;
		int camera_y = player->position.y - y_resolution / 2;
	}

	// Create an empty visible map filled with spaces
	std::vector<std::string> visible(y_resolution, std::string(x_resolution, ' '));

	vector<Actor>* actors = GetActors();

	// Overlay actors onto the visible map, including walls
	for (const Actor& actor : *actors) {
		int ax = actor.position.x;
		int ay = actor.position.y;

		// Check if the actor is within the camera's view
		if (ax >= camera_x && ax < camera_x + x_resolution &&
			ay >= camera_y && ay < camera_y + y_resolution) {

			int vx = ax - camera_x; // Actor's x position in the visible map
			int vy = ay - camera_y; // Actor's y position in the visible map

			// Ensure we are within bounds before placing the actor
			if (vy >= 0 && vy < y_resolution && vx >= 0 && vx < x_resolution) {
				visible[vy][vx] = actor.view; // Place the actor character in the visible map
			}
		}
	}

	// Convert the visible map to a string
	std::string rendered_map;
	for (const std::string& row : visible) {
		rendered_map += row + '\n';
	}

	return rendered_map;
}


Actor* Engine::GetPlayer() { 
	return scene.GetPlayer().get();
}

std::vector<Actor>* Engine::GetActors() { 
	return &scene.GetActors();
}

std::unordered_set<int>* Engine::GetScoreActors()
{
	return &scene.GetScoreActors();
}

std::unordered_map<uint64_t, int>* Engine::GetBlockingPositionsToNum()
{
	return &scene.GetBlockingPositionsToNum();
}