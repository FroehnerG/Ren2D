#include <iostream>
#include <filesystem>
#include <cstdlib> // For exit(0)
#include "SceneDB.h"
#include "TemplateDB.h"
#include "EngineUtils.h"

using std::cout;
namespace fs = std::filesystem;

void SceneDB::LoadActors(rapidjson::Document& scene_json, SDL_Renderer* renderer, ImageDB* imageDB, AudioDB* audioDB)
{
	TemplateDB& templateDB = TemplateDB::GetInstance();
	actors.reserve(10000);

	for (auto& actor_json : scene_json["actors"].GetArray()) {
		Actor actor;
		actor.id = current_actor_id;

		if (actor_json.HasMember("template")) {
			// If template exists, use template and assign values to actor
			string template_name = actor_json["template"].GetString();

			// Will exit program if template file does not exist
			LoadTemplate(template_name, renderer, imageDB, audioDB, current_actor_id);

			actor = templateDB.UseTemplate(template_name);
		}

		actor.ParseActorFromJson(renderer, imageDB, audioDB, actor_json, current_actor_id);

		if (actor.blocking) {
			uint64_t composite_position = EngineUtils::CreateCompositeKey(actor.position);
			blocking_positions_to_num[composite_position]++;
		}

		current_actor_id++;

		actors.push_back(std::move(actor));

		if (actors.back().actor_name == "player") {
			player = std::shared_ptr<Actor>(actors.data() + (actors.size() - 1), [](Actor*) {});
		}

	}
}

void SceneDB::LoadTemplate(string template_name, SDL_Renderer* renderer, ImageDB* imageDB, AudioDB* audioDB, int current_actor_id)
{
	TemplateDB& templateDB = TemplateDB::GetInstance();
	string template_path = "resources/actor_templates/" + template_name + ".template";

	if (!fs::exists(template_path)) {
		cout << "error: template " + template_name + " is missing";
		exit(0);
	}

	rapidjson::Document template_json;
	EngineUtils::ReadJsonFile(template_path, template_json);

	templateDB.LoadTemplate(template_name, renderer, imageDB, audioDB, template_json, current_actor_id);
}

void SceneDB::SortRenderActors(bool is_map_created, Actor* moved_actor)
{
	if (!is_map_created) {
		sorted_actors.clear();

		for (const auto& actor : actors) {
			float primary = actor.render_order.value_or(actor.position.y);
			sorted_actors.insert({ RenderKey{ primary, actor.id }, &actor });
		}

		return;
	}

	// Handle update for single moved actor
	if (moved_actor != nullptr) {
		// Step 1: Find the old entry
		for (auto it = sorted_actors.begin(); it != sorted_actors.end(); ++it) {
			if (it->second == moved_actor) {
				sorted_actors.erase(it);  // Step 2: Erase the old entry
				break;
			}
		}

		// Step 3: Recompute render key
		float new_primary = moved_actor->render_order.value_or(moved_actor->position.y);
		RenderKey new_key = { new_primary, moved_actor->id };

		// Step 4: Insert updated entry
		sorted_actors.insert({ new_key, moved_actor });
	}
}

void SceneDB::Reset()
{
	position_to_actor.clear();
	blocking_positions_to_num.clear();
	score_actors.clear();
	actors.clear();
	player.reset();  // Reset shared pointer
	current_actor_id = 0;
}

std::multimap<RenderKey, const Actor*>* SceneDB::GetSortedActors()
{
	return &sorted_actors;
}

std::unordered_set<int>& SceneDB::GetScoreActors()
{
	return score_actors;
}

std::unordered_map<uint64_t, int>& SceneDB::GetBlockingPositionsToNum()
{
	return blocking_positions_to_num;
}

std::vector<Actor>& SceneDB::GetActors()
{
	return actors;
}

std::shared_ptr<Actor> SceneDB::GetPlayer()
{
	return player;
}
