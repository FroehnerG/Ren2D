#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "glm/glm.hpp"
#include "Actor.h"
#include "rapidjson/document.h"
#include "TemplateDB.h"
#include "ImageDB.h"

using std::vector, glm::ivec2, std::string;

class SceneDB {
public:
	void LoadActors(rapidjson::Document& scene_json, SDL_Renderer* renderer, ImageDB* imageDB);
	void LoadTemplate(string template_name, SDL_Renderer* renderer, ImageDB* imageDB, int current_actor_id);
	void Reset();
	std::unordered_set<int>& GetScoreActors();
	std::unordered_map<uint64_t, int>& GetBlockingPositionsToNum();
	std::vector<Actor>& GetActors();
	std::shared_ptr<Actor> GetPlayer();
private:
	std::unordered_map<uint64_t, Actor> position_to_actor;
	std::unordered_map<uint64_t, int> blocking_positions_to_num;
	std::unordered_set<int> score_actors;
	vector<Actor> actors;
	std::shared_ptr<Actor> player;
	int current_actor_id = 0;
};
