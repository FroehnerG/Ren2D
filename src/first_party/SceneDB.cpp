#include <iostream>
#include <filesystem>
#include <cstdlib> // For exit(0)
#include "SceneDB.h"
#include "TemplateDB.h"
#include "EngineUtils.h"

using std::cout;
namespace fs = std::filesystem;

void SceneDB::LoadActors(rapidjson::Document& scene_json, SDL_Renderer* renderer, ImageDB* imageDB)
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
			LoadTemplate(template_name, renderer, imageDB, current_actor_id);

			actor = templateDB.UseTemplate(template_name);
		}

		actor.ParseActorFromJson(renderer, imageDB, actor_json, current_actor_id);

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

void SceneDB::LoadTemplate(string template_name, SDL_Renderer* renderer, ImageDB* imageDB, int current_actor_id)
{
	TemplateDB& templateDB = TemplateDB::GetInstance();
	string template_path = "resources/actor_templates/" + template_name + ".template";

	if (!fs::exists(template_path)) {
		cout << "error: template " + template_name + " is missing";
		exit(0);
	}

	rapidjson::Document template_json;
	EngineUtils::ReadJsonFile(template_path, template_json);

	templateDB.LoadTemplate(template_name, renderer, imageDB, template_json, current_actor_id);
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
