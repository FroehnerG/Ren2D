#include <functional>  // For std::hash
#include <iostream>
#include "TemplateDB.h"

void TemplateDB::LoadTemplate(std::string template_name, SDL_Renderer* renderer, ImageDB* imageDB, AudioDB* audioDB, rapidjson::Document& template_json, int current_actor_id)
{
	uint64_t actor_template_hash = EngineUtils::HashString(template_name);

	// If template already exists, return
	if (actor_templates.find(actor_template_hash) != actor_templates.end()) {
		return;
	}

	Actor actor_template;

	actor_template.ParseActorFromJson(renderer, imageDB, audioDB, template_json, current_actor_id);

	actor_templates[actor_template_hash] = actor_template;
}

Actor TemplateDB::UseTemplate(std::string template_name)
{
	uint64_t actor_template_hash = EngineUtils::HashString(template_name);

	auto it = actor_templates.find(actor_template_hash);
	if (it == actor_templates.end()) {
		std::cout << "template " << template_name << " is missing";
		std::exit(0);
	}

	// Return a copy of the template
	return it->second;
}
