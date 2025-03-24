#include <iostream>
#include <filesystem>
#include "rapidjson/document.h"
#include "EngineUtils.h"
#include "Engine.h"
#include "Input.h"

using std::string, std::cin, std::cout;
namespace fs = std::filesystem;

const string prompt = "Please make a decision...\n";
const string options = "Your options are \"n\", \"e\", \"s\", \"w\", \"quit\"\n";

const vec2 ADJACENT_OFFSETS[8] = {
	vec2(0, -1),
	vec2(0, 1),
	vec2(-1, 0),
	vec2(1, 0), // Cardinal directions
	vec2(-1, -1),
	vec2(-1, 1),
	vec2(1, -1),
	vec2(1, 1) // Diagonal directions
};

Engine::Engine(rapidjson::Document& game_config)
{
	rapidjson::Document scene_json;

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

	if (game_config.HasMember("player_movement_speed")) {
		player_movement_speed = game_config["player_movement_speed"].GetFloat();
	}

	if (game_config.HasMember("initial_scene")) {
		string scene_name = game_config["initial_scene"].GetString();

		string scene_path = "resources/scenes/" + scene_name + ".scene";

		if (!fs::exists(scene_path)) {
			cout << "error: scene " + scene_name + " is missing";
			exit(0);
		}

		EngineUtils::ReadJsonFile(scene_path, scene_json);
	}
	else {
		cout << "error: initial_scene unspecified";
		exit(0);
	}

	window = CreateWindow();
	renderer.SetRenderer(window);
	SDL_SetRenderDrawColor(renderer.GetRenderer(), renderer.GetColor("red"), renderer.GetColor("green"), renderer.GetColor("blue"), 255);
	SDL_RenderClear(renderer.GetRenderer());
	images.LoadImages(game_config, renderer.GetRenderer(), true, "", -1); // Load intro
	images.LoadImages(game_config, renderer.GetRenderer(), false, "game_over_good_image", -1);
	images.LoadImages(game_config, renderer.GetRenderer(), false, "game_over_bad_image", -1);
	text.LoadText(game_config, &images, true);
	renderer.SetFont(&text);
	audio.LoadAudio(game_config, "intro_bgm", true);
	audio.LoadAudio(game_config, "gameplay_audio", false);
	audio.LoadAudio(game_config, "game_over_good_audio", false);
	audio.LoadAudio(game_config, "game_over_bad_audio", false);

	scene.LoadActors(scene_json, renderer.GetRenderer(), &images);

	if (GetPlayer() != nullptr) {
		images.LoadImages(game_config, renderer.GetRenderer(), false, "hp_image", -1);
	}
}

void Engine::GameLoop()
{
	if (game_start_message != "")
		cout << game_start_message << '\n';

	while (is_running) {
		current_frame = Helper::GetFrameNumber();  // Call this every frame
		SDL_Event e;
		while (Helper::SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				Helper::SDL_RenderPresent(renderer.GetRenderer());
				exit(0);
			}

			Input::ProcessEvent(e);
		}

		if (images.IsIntroPlaying() || text.IsIntroPlaying()) {
			audio.PlayMusic(true);
			PlayIntro();
		}

		if (!images.IsIntroPlaying() && !text.IsIntroPlaying()) {
			if (audio.HasIntroMusic() && audio.intro_music_playing) {
				audio.HaltMusic();
				audio.intro_music_playing = false;
			}

			if (audio.HasGameplayMusic() && !audio.gameplay_music_playing) {

				audio.PlayMusic(false);
				audio.gameplay_music_playing = true;
			}

			HandlePlayerMovement();
		}

		if (!images.IsIntroPlaying() && !text.IsIntroPlaying() && !game_over_bad && !game_over_good) {
			Update();
		}

		vector<string> dialogue;

		Render(&dialogue);

		if (game_over_good) {
			if (!audio.game_over_music_playing) {
				AudioHelper::Mix_HaltChannel(0);

				if (audio.has_game_over_good_audio) {
					audio.PlayGameOverMusic(true);
				}
				audio.game_over_music_playing = true;
			}
			renderer.RenderEnd(images.GetGameOverImage(true));
		}
		else if (game_over_bad) {
			if (!audio.game_over_music_playing) {
				AudioHelper::Mix_HaltChannel(0);

				if (audio.has_game_over_bad_audio) {
					audio.PlayGameOverMusic(false);
				}
				audio.game_over_music_playing = true;
			}
			renderer.RenderEnd(images.GetGameOverImage(false));
		}
		else if (GetPlayer() != nullptr) {
			renderer.Render(GetActors(), &dialogue, GetPlayer(), x_resolution, y_resolution, images.GetHPImage(), player_health, score);
			//cout << current_frame << '\n';
		}
		else {
			renderer.Render(GetActors(), &dialogue, GetPlayer(), x_resolution, y_resolution, nullptr, std::nullopt, score);
			//cout << current_frame << '\n';
		}
	}
}

void Engine::PlayIntro()
{
	// **Intro Sequence Loop**
	while (images.IsIntroPlaying() || text.IsIntroPlaying()) {
		SDL_Event e;
		while (Helper::SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				Helper::SDL_RenderPresent(renderer.GetRenderer());
				exit(0);
			}

			// Handle advancing intro images
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.scancode == SDL_SCANCODE_SPACE ||
					e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
					images.AdvanceIntro();
					text.AdvanceIntroText();
				}
			}

			if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
				images.AdvanceIntro();
				text.AdvanceIntroText();
			}
		}

		if (!images.IsIntroPlaying() && !text.IsIntroPlaying()) {
			if (audio.HasIntroMusic() && audio.intro_music_playing) {
				audio.HaltMusic();
				audio.intro_music_playing = false;
			}

			if (audio.HasGameplayMusic() && !audio.gameplay_music_playing) {
				audio.PlayMusic(false);
				audio.gameplay_music_playing = true;
			}
			return;
		}


		// **Render only intro image**
		SDL_SetRenderDrawColor(renderer.GetRenderer(), renderer.GetColor("red"), renderer.GetColor("green"), renderer.GetColor("blue"), 255);
		SDL_RenderClear(renderer.GetRenderer());
		renderer.RenderIntro(&images, &text, y_resolution);
		Helper::SDL_RenderPresent(renderer.GetRenderer());
	}
}

void Engine::HandlePlayerMovement() {
	if (GetPlayer() == nullptr) {
		return;
	}

	vec2 direction = vec2(0.0f, 0.0f);

	// Accumulate direction from WASD/Arrow keys
	if (Input::GetKey(SDL_SCANCODE_W) || Input::GetKey(SDL_SCANCODE_UP)) {
		direction.y -= 1.0f;
	}
	if (Input::GetKey(SDL_SCANCODE_S) || Input::GetKey(SDL_SCANCODE_DOWN)) {
		direction.y += 1.0f;
	}
	if (Input::GetKey(SDL_SCANCODE_A) || Input::GetKey(SDL_SCANCODE_LEFT)) {
		direction.x -= 1.0f;
	}
	if (Input::GetKey(SDL_SCANCODE_D) || Input::GetKey(SDL_SCANCODE_RIGHT)) {
		direction.x += 1.0f;
	}

	// Only move if direction is non-zero
	if (direction.x != 0.0f || direction.y != 0.0f) {
		direction = glm::normalize(direction); // Normalize to maintain consistent speed
		UpdatePlayerPosition(direction * player_movement_speed);
	}
}



bool Engine::IsPositionValid(vec2 position)
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
		if (actor.actor_name == "player") {
			continue;
		}

		if (actor.velocity != vec2(0.0f, 0.0f)) {
			vec2 new_actor_position = actor.position + actor.velocity;

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
			else {
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

	scene.LoadActors(scene_json, renderer.GetRenderer(), &images);
}

SDL_Window* Engine::CreateWindow()
{
	std::vector<char> game_title_cstr(game_title.begin(), game_title.end());
	game_title_cstr.push_back('\0'); // Ensure null termination

	return Helper::SDL_CreateWindow(game_title_cstr.data(), 500, 500, x_resolution, y_resolution, SDL_WINDOW_SHOWN);
}

ivec2 Engine::InvertVelocity(vec2 velocity)
{
	return vec2(velocity.x * -1, velocity.y * -1);
}

void Engine::Update()
{
	MoveNPCs();
}

void Engine::Render(vector<string>* dialogue)
{
	//cout << RenderMap();
	ShowNPCDialogue(dialogue);
}

void Engine::UpdatePlayerPosition(vec2 direction)
{
	vec2 new_position = GetPlayer()->position + direction;

	if (IsPositionValid(new_position)) {
		if (GetPlayer()->blocking) {
			uint64_t composite_position = EngineUtils::CreateCompositeKey(new_position);
			uint64_t old_composite_position = EngineUtils::CreateCompositeKey(GetPlayer()->position);

			// Add new position and remove old position to blocking map
			(*GetBlockingPositionsToNum())[composite_position]++;
			(*GetBlockingPositionsToNum())[old_composite_position]--;
		}

		GetPlayer()->position = new_position;
	}
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

	glm::vec2 cam_offset = glm::vec2(0.0f, 0.0f);

	if (rendering_config.HasMember("cam_offset_x")) {
		cam_offset.x = rendering_config["cam_offset_x"].GetFloat();
	}

	if (rendering_config.HasMember("cam_offset_y")) {
		cam_offset.y = rendering_config["cam_offset_y"].GetFloat();
	}

	renderer.SetCamOffset(cam_offset);

	if (rendering_config.HasMember("zoom_factor")) {
		float zoom_factor = rendering_config["zoom_factor"].GetFloat();
		renderer.SetZoomFactor(zoom_factor);
	}
}

void Engine::ShowScoreAndHealth()
{
	cout << "health : " << player_health << ", score : " << score << '\n';
}

bool Engine::IsNPCAdjacent(vec2 NPC_position)
{
	// Check all neighbors
	Actor* player = GetPlayer();

	for (const auto& offset : ADJACENT_OFFSETS) {
		vec2 neighbor_position = player->position + offset;

		if (NPC_position == neighbor_position) {
			return true; // Found an adjacent actor
		}
	}

	return false;
}

bool Engine::IsNPCInSameCell(vec2 NPC_position)
{
	if (NPC_position == GetPlayer()->position) {
		return true;
	}

	return false;
}

void Engine::ShowNPCDialogue(vector<string>* dialogue)
{
	vector<Actor>* actors = GetActors();

	if (GetPlayer() == nullptr) {
		return;
	}

	bool skip_rendering = false;  // Flag to avoid rendering dialogue on scene transition

	for (const auto& actor : *actors) {
		std::string message;

		if (IsNPCInSameCell(actor.position) && actor.contact_dialogue != "") {
			message = actor.contact_dialogue;
		}
		else if (IsNPCAdjacent(actor.position) && actor.nearby_dialogue != "") {
			message = actor.nearby_dialogue;
		}

		if (!message.empty()) {
			// Check for "proceed to" before rendering
			if (message.find("proceed to") != std::string::npos) {
				CheckNPCDialogue(message, actor.id);
				skip_rendering = true;  // Don't show dialogue for this frame
				break;  // Stop processing further to avoid one-frame hitch
			}

			dialogue->push_back(message);  // Queue message for rendering
			CheckNPCDialogue(message, actor.id);

			if (game_over_good || game_over_bad) {
				dialogue->clear();
				return;
			}
		}
	}

	if (skip_rendering) {
		next_scene = false;
		LoadScene(next_scene_name);
		dialogue->clear();
		return;  // Skip rendering this frame
	}

	// Game over rendering
	if (!is_running) {
		ShowScoreAndHealth();

		if (game_over_good && !game_over_good_message.empty()) {
			//renderer.DrawText(game_over_good_message, 16, { 255, 255, 255, 255 }, 25, y_resolution - 100);
			return;
		}

		if (!game_over_bad_message.empty()) {
			//renderer.DrawText(game_over_bad_message, 16, { 255, 0, 0, 255 }, 25, y_resolution - 100);
		}
	}
}


void Engine::RenderNPCDialogue(vector<string>* dialogue) {

}

void Engine::CheckNPCDialogue(std::string& dialogue, int actor_id)
{
	std::unordered_set<int>* score_actors = GetScoreActors();

	const std::string health_down = "health down";
	const std::string score_up = "score up";
	const std::string you_win = "you win";
	const std::string game_over = "game over";
	const std::string proceed_to = "proceed to";

	if (dialogue.find(health_down) != std::string::npos) {
		if (current_frame >= last_damage_frame + 180) {
			player_health--;
			last_damage_frame = current_frame;

			if (player_health <= 0) {
				if (!images.HasGameOverImage(false)) {
					exit(0);
				}

				game_over_bad = true;
				return;
			}
		}
	}
	else if (dialogue.find(score_up) != std::string::npos && score_actors->find(actor_id) == score_actors->end()) {
		score++;
		score_actors->insert(actor_id);
	}
	else if (dialogue.find(you_win) != std::string::npos) {
		if (!images.HasGameOverImage(true)) {
			exit(0);
		}

		game_over_good = true;
		return;
	}
	else if (dialogue.find(game_over) != std::string::npos) {
		if (current_frame >= last_damage_frame + 180) {
			if (!images.HasGameOverImage(false)) {
				exit(0);
			}

			game_over_bad = true;
			return;
		}
	}
	else if (dialogue.find(proceed_to) != std::string::npos) {
		next_scene = true;
		next_scene_name = EngineUtils::ObtainWordAfterPhrase(dialogue, proceed_to);
	}
}


/*
std::string Engine::RenderMap()
{
	// Compute camera bounds directly based on the player's position
	Actor* player = GetPlayer();

	int camera_x = 0;
	int camera_y = 0;

	if (player != nullptr) {
		camera_x = player->position.x - x_resolution / 2;
		camera_y = player->position.y - y_resolution / 2;
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
*/


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