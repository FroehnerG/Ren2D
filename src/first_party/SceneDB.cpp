#include <iostream>
#include <filesystem>
#include <cstdlib> // For exit(0)
#include "SceneDB.h"
#include "TemplateDB.h"
#include "EngineUtils.h"

using std::cout;
namespace fs = std::filesystem;

void SceneDB::LoadActors(rapidjson::Document& scene_json)
{
	actors.reserve(10000);

	for (const auto& actor_json : scene_json["actors"].GetArray()) {
		Actor actor;

		if (actor_json.HasMember("template")) {
			LoadTemplate(actor_json);

			string template_name = actor_json["template"].GetString();
			actor = templateDB->UseTemplate(template_name);
		}

		if (actor_json.HasMember("name")) {
			actor.actor_name = actor_json["name"].GetString();
		}

		if (actor_json.HasMember("view")) {
			actor.view = actor_json["view"].GetString()[0];
		}

		if (actor_json.HasMember("x")) {
			actor.position.x = actor_json["x"].GetInt();
		}

		if (actor_json.HasMember("y")) {
			actor.position.y = actor_json["y"].GetInt();
		}

		if (actor_json.HasMember("vel_x")) {
			actor.velocity.x = actor_json["vel_x"].GetInt();
		}

		if (actor_json.HasMember("vel_y")) {
			actor.velocity.y = actor_json["vel_y"].GetInt();
		}

		if (actor_json.HasMember("blocking")) {
			actor.blocking = actor_json["blocking"].GetBool();

			if (actor.blocking) {
				uint64_t composite_position = EngineUtils::CreateCompositeKey(actor.position);
				blocking_positions_to_num[composite_position]++;
			}
		}

		if (actor_json.HasMember("nearby_dialogue")) {
			actor.nearby_dialogue = actor_json["nearby_dialogue"].GetString();
		}

		if (actor_json.HasMember("contact_dialogue")) {
			actor.contact_dialogue = actor_json["contact_dialogue"].GetString();
		}

		actor.id = current_actor_id;
		current_actor_id++;

		actors.push_back(std::move(actor));

		if (actors.back().actor_name == "player") {
			player = std::shared_ptr<Actor>(actors.data() + (actors.size() - 1), [](Actor*) {});
		}

	}
}

void SceneDB::LoadTemplate(const rapidjson::Value& actor_json)
{
	string template_name = actor_json["template"].GetString();

	string template_path = "resources/actor_templates/" + template_name + ".template";

	if (!fs::exists(template_path)) {
		cout << "error: template " + template_name + " is missing";
		exit(0);
	}

	rapidjson::Document template_json;
	EngineUtils::ReadJsonFile(template_path, template_json);

	templateDB->LoadTemplate(template_name, template_json);
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
