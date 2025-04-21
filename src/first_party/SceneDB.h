#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <memory>
#include "glm/glm.hpp"
#include "Actor.h"
#include "rapidjson/document.h"
#include "TemplateDB.h"
#include "ImageDB.h"

using std::vector, glm::ivec2, std::string;

struct RenderKey {
	float sort_value; // render_order or position.y
	int actor_id;

	bool operator<(const RenderKey& other) const {
		if (sort_value != other.sort_value)
			return sort_value < other.sort_value;
		return actor_id < other.actor_id;
	}
};

class SceneDB {
public:
	void LoadActors(rapidjson::Document& scene_json, SDL_Renderer* renderer, ImageDB* imageDB, AudioDB* audioDB, EventListener* eventListener);
	void LoadTemplate(string template_name, SDL_Renderer* renderer, ImageDB* imageDB, AudioDB* audioDB, EventListener* eventListener, int current_actor_id);
	void SortRenderActors(bool is_map_created, Actor* moved_actor);
	void Reset();
	std::multimap<RenderKey, const Actor*>* GetSortedActors();
	std::unordered_set<int>& GetScoreActors();
	std::unordered_map<uint64_t, int>& GetBlockingPositionsToNum();
	std::vector<Actor>& GetActors();
	std::shared_ptr<Actor> GetPlayer();
private:
	std::multimap<RenderKey, const Actor*> sorted_actors;
	std::unordered_map<uint64_t, Actor> position_to_actor;
	std::unordered_map<uint64_t, int> blocking_positions_to_num;
	std::unordered_set<int> score_actors;
	vector<Actor> actors;
	std::shared_ptr<Actor> player;
	int current_actor_id = 0;
};
