#pragma once
#include <unordered_map>
#include <string>
#include "rapidjson/document.h"
#include "EngineUtils.h"
#include "SceneDB.h"

class TemplateDB {
public:
	void LoadTemplate(std::string template_name, rapidjson::Document& template_json);
	static uint64_t HashTemplateName(string template_name);
	Actor UseTemplate(std::string template_name);
private:
	std::unordered_map<uint64_t, Actor> actor_templates;
};
