#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "glm/glm.hpp"
#include "rapidjson/document.h"

using std::vector, glm::ivec2, std::string;

struct Actor
{
public:
	int id = -1;
	std::string actor_name = "";
	char view = '?';
	glm::ivec2 position = glm::ivec2(0, 0);
	glm::ivec2 velocity = glm::ivec2(0, 0);
	bool blocking = false;
	std::string nearby_dialogue = "";
	std::string contact_dialogue = "";

	Actor(std::string actor_name, char view, glm::ivec2 position, glm::ivec2 initial_velocity,
		bool blocking, std::string nearby_dialogue, std::string contact_dialogue)
		: actor_name(actor_name), view(view), position(position), velocity(initial_velocity), blocking(blocking), nearby_dialogue(nearby_dialogue), contact_dialogue(contact_dialogue) {
		id = -1;
	}

	Actor() {}
};


class SceneDB {
public:
	void LoadActors(rapidjson::Document& scene_json);
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