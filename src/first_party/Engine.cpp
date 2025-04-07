#include <iostream>
#include <filesystem>
#include "rapidjson/document.h"
#include "EngineUtils.h"
#include "Engine.h"
#include "Input.h"

using std::string, std::cin, std::cout;
namespace fs = std::filesystem;

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
	audio.LoadAudio(game_config, "score_sfx", false);

	scene.LoadActors(scene_json, renderer.GetRenderer(), &images, &audio);
	scene.SortRenderActors(false, nullptr);

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
				if (game_over_good || game_over_bad) {
					Helper::SDL_RenderPresent(renderer.GetRenderer());
				}
				else {
					Update();
					if (GetPlayer() != nullptr) {
						renderer.Render(scene.GetSortedActors(), &dialogue, GetPlayer(), x_resolution, y_resolution, images.GetHPImage(), player_health, score);
						//cout << current_frame << '\n';
					}
					else {
						renderer.Render(scene.GetSortedActors(), &dialogue, GetPlayer(), x_resolution, y_resolution, nullptr, std::nullopt, score);
						//cout << current_frame << '\n';
					}
				}
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

			if (!game_over_bad && !game_over_good) {
				//HandlePlayerMovement();
				Update();
			}
		}

		Render();

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
			renderer.Render(scene.GetSortedActors(), &dialogue, GetPlayer(), x_resolution, y_resolution, images.GetHPImage(), player_health, score);
			//cout << current_frame << '\n';
		}
		else {
			renderer.Render(scene.GetSortedActors(), &dialogue, GetPlayer(), x_resolution, y_resolution, nullptr, std::nullopt, score);
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
	Actor* player = GetPlayer();

	if (player == nullptr) {
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
	
	if (direction != vec2(0.0f, 0.0f)) {
		player->velocity = direction;
	}
	else {
		player->velocity = vec2(0.0f, 0.0f);
	}

	// Only move if direction is non-zero
	if (direction.x != 0.0f || direction.y != 0.0f) {
		direction = glm::normalize(direction); // Normalize to maintain consistent speed

		if (current_frame % 20 == 0) {
			audio.PlayActorSFX(-1, "step_sfx", current_frame % 48 + 2);
		}

		if (direction.x > 0 && renderer.GetXFlipOnMovement()) {
			player->transform_scale.x = glm::abs(player->transform_scale.x);
		}
		else if (direction.x < 0 && renderer.GetXFlipOnMovement()) {
			player->transform_scale.x = -1.0f * glm::abs(player->transform_scale.x);
		}

		if (direction.y < 0 && player->view_image_back) {
			player->show_view_image_back = true;
		}
		else if (direction.y > 0 && player->view_image_back) {
			player->show_view_image_back = false;
		}
	}

	UpdatePlayerPosition(direction * player_movement_speed);
}

bool Engine::IsPositionValid(Actor* actor, vec2 new_position)
{
	vector<Actor>* actors = GetActors();
	vec2 old_position = actor->position;
	actor->position = new_position;

	for (auto& other : *actors) {
		if (actor->id == other.id) {
			continue;
		}

		if (current_frame == 50 && other.actor_name == "player") {
			cout << "test2";
		}

		if (actor->AreBoxesOverlapping(other, false)) {
			actor->InsertCollidingActor(&other);
			other.InsertCollidingActor(actor);
			actor->position = old_position;
		}
	}

	if (actor->colliding_actors_this_frame.empty()) {
		return true;
	}

	return false;
}

void Engine::MoveNPCs()
{
	vector<Actor>* actors = GetActors();
	// Go through each actor in actor vector and move them
	for (auto& actor : *actors) {
		// Only show damage sprite for 30 frames after last_damage_frame
		if (current_frame >= last_damage_frame + 30) {
			actor.show_view_image_damage = false;
		}

		if (actor.actor_name == "player") {
			HandlePlayerMovement();
			continue;
		}

		if (actor.velocity != vec2(0.0f, 0.0f)) {

			vec2 new_actor_position = actor.position + actor.velocity;

			if (actor.direction_changed) {
				if (actor.velocity.x > 0 && renderer.GetXFlipOnMovement()) {
					actor.transform_scale.x = glm::abs(actor.transform_scale.x);
				}
				else if (actor.velocity.x < 0 && renderer.GetXFlipOnMovement()) {
					actor.transform_scale.x = -1.0f * glm::abs(actor.transform_scale.x);
				}

				if (actor.velocity.y < 0 && actor.view_image_back) {
					actor.show_view_image_back = true;
				}
				else if (actor.velocity.y > 0 && actor.view_image_back) {
					actor.show_view_image_back = false;
				}

				actor.direction_changed = false;
			}

			// If actor can move to new location, move them
			if (IsPositionValid(&actor, new_actor_position)) {
				scene.SortRenderActors(true, &actor);
			}
			else {
				actor.velocity = InvertVelocity(actor.velocity);
				actor.direction_changed = true;
			}
		}
	}
}

void Engine::ClearCollidingActorsSet()
{
	vector<Actor>* actors = GetActors();

	for (auto& actor : *actors) {
		actor.colliding_actors_this_frame.clear();
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

	scene.LoadActors(scene_json, renderer.GetRenderer(), &images, &audio);
	scene.SortRenderActors(false, nullptr);
}

SDL_Window* Engine::CreateWindow()
{
	std::vector<char> game_title_cstr(game_title.begin(), game_title.end());
	game_title_cstr.push_back('\0'); // Ensure null termination

	return Helper::SDL_CreateWindow(game_title_cstr.data(), 500, 500, x_resolution, y_resolution, SDL_WINDOW_SHOWN);
}

vec2 Engine::InvertVelocity(vec2 velocity)
{
	return vec2(velocity.x * -1.0f, velocity.y * -1.0f);
}

void Engine::Update()
{
	MoveNPCs();
	CheckTriggerActors();
}

void Engine::Render()
{
	//cout << RenderMap();
	ShowNPCDialogue();
	ClearCollidingActorsSet();
}

void Engine::UpdatePlayerPosition(vec2 direction)
{
	vec2 new_position = GetPlayer()->position + direction;

	if (IsPositionValid(GetPlayer(), new_position)) {
		scene.SortRenderActors(true, GetPlayer());
	}
}

void Engine::CheckTriggerActors()
{
	Actor* player = GetPlayer();

	if (!player->trigger) {
		return;
	}

	vector<Actor>* actors = GetActors();

	for (auto& actor : *actors) {
		string message = actor.nearby_dialogue;

		if (player->AreBoxesOverlapping(actor, true) && !message.empty()) {
			dialogue.push_back(message);
			CheckNPCDialogue(message, &actor);
			audio.PlayActorSFX(actor.id, "nearby", current_frame % 48 + 2);
			if (next_scene) {
				next_scene = false;
				LoadScene(next_scene_name);
				dialogue.clear();
				return;
			}

			if (game_over_good || game_over_bad) {
				dialogue.clear();
				return;
			}
		}
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

	if (rendering_config.HasMember("x_scale_actor_flipping_on_movement")) {
		renderer.SetXFlipOnMovement(rendering_config["x_scale_actor_flipping_on_movement"].GetBool());
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

	if (rendering_config.HasMember("cam_ease_factor")) {
		float cam_ease_factor = rendering_config["cam_ease_factor"].GetFloat();
		renderer.SetCamEaseFactor(cam_ease_factor);
	}

	renderer.SetCameraPosition(GetPlayer());
}

void Engine::ShowNPCDialogue()
{
	vector<Actor>* actors = GetActors();

	if (GetPlayer() == nullptr) {
		return;
	}

	Actor* player = GetPlayer();

	std::unordered_set<Actor*> colliding_actors = GetPlayer()->colliding_actors_this_frame;

	bool skip_rendering = false;  // Flag to avoid rendering dialogue on scene transition

	for (const auto& actor : colliding_actors) {
		std::string message = actor->contact_dialogue;

		if (!message.empty()) {
			CheckNPCDialogue(message, actor);

			if (next_scene) {
				next_scene = false;
				LoadScene(next_scene_name);
				dialogue.clear();
				return;
			}

			if (game_over_good || game_over_bad) {
				dialogue.clear();
				return;
			}
		}
	}

	if (player->view_image_damage && current_frame < last_damage_frame + 30) {
		player->show_view_image_damage = true;
	}
	else {
		player->show_view_image_damage = false;
	}
}

void Engine::CheckNPCDialogue(std::string& dialogue, Actor* actor)
{
	std::unordered_set<int>* score_actors = GetScoreActors();
	Actor* player = GetPlayer();

	const std::string health_down = "health down";
	const std::string score_up = "score up";
	const std::string you_win = "you win";
	const std::string game_over = "game over";
	const std::string proceed_to = "proceed to";

	if (dialogue.find(health_down) != std::string::npos) {
		if (current_frame >= last_damage_frame + 180) {
			player_health--;

			audio.PlayActorSFX(-1, "damage_sfx", current_frame % 48 + 2);

			last_damage_frame = current_frame;

			if (player_health <= 0) {
				if (!images.HasGameOverImage(false)) {
					exit(0);
				}

				game_over_bad = true;
				return;
			}
			// Only show damage sprite for 30 frames after last_damage_frame
			if (actor->view_image_damage && current_frame < last_damage_frame + 30) {
				actor->show_view_image_damage = true;
			}
			else {
				actor->show_view_image_damage = false;
			}
		}
	}
	else if (dialogue.find(score_up) != std::string::npos && score_actors->find(actor->id) == score_actors->end()) {
		score++;
		score_actors->insert(actor->id);

		audio.PlayActorSFX(-1, "score_sfx", 1);
		
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