#include <functional>  // For std::hash
#include <iostream>
#include "TemplateDB.h"

void TemplateDB::LoadTemplate(std::string template_name, rapidjson::Document& template_json)
{
	uint64_t actor_template_hash = HashTemplateName(template_name);

	// If template already exists, return
	if (actor_templates.find(actor_template_hash) != actor_templates.end()) {
		return;
	}

	Actor actor_template;

	if (template_json.HasMember("name")) {
		actor_template.actor_name = template_json["name"].GetString();
	}

	if (template_json.HasMember("view")) {
		actor_template.view = template_json["view"].GetString()[0];
	}

	if (template_json.HasMember("x")) {
		actor_template.position.x = template_json["x"].GetInt();
	}

	if (template_json.HasMember("y")) {
		actor_template.position.y = template_json["y"].GetInt();
	}

	if (template_json.HasMember("vel_x")) {
		actor_template.velocity.x = template_json["vel_x"].GetInt();
	}

	if (template_json.HasMember("vel_y")) {
		actor_template.velocity.y = template_json["vel_y"].GetInt();
	}

	if (template_json.HasMember("blocking")) {
		actor_template.blocking = template_json["blocking"].GetBool();
	}

	if (template_json.HasMember("nearby_dialogue")) {
		actor_template.nearby_dialogue = template_json["nearby_dialogue"].GetString();
	}

	if (template_json.HasMember("contact_dialogue")) {
		actor_template.contact_dialogue = template_json["contact_dialogue"].GetString();
	}

	actor_templates[actor_template_hash] = actor_template;
}

uint64_t TemplateDB::HashTemplateName(string template_name)
{
	// Hash the string before inserting it
	std::hash<std::string> hasher;
	uint64_t key = hasher(template_name);

	return key;
}

Actor TemplateDB::UseTemplate(std::string template_name)
{
	uint64_t actor_template_hash = HashTemplateName(template_name);

	auto it = actor_templates.find(actor_template_hash);
	if (it == actor_templates.end()) {
		std::cout << "template " << template_name << " is missing";
		std::exit(0);
	}

	// Return a copy of the template
	return it->second;
}
