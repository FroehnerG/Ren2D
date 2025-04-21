#pragma once
#include <unordered_map>
#include "Actor.h"
#include "rapidjson/document.h"
#include "EngineUtils.h"
#include "ImageDB.h"

using std::string;

class TemplateDB {
public:
    // Delete copy constructor and assignment operator to prevent multiple instances
    TemplateDB(const TemplateDB&) = delete;
    TemplateDB& operator=(const TemplateDB&) = delete;

    // Public method to access the single instance
    static TemplateDB& GetInstance() {
        static TemplateDB instance;  // Only created once
        return instance;
    }

    void LoadTemplate(std::string template_name, SDL_Renderer* renderer, ImageDB* imageDB, AudioDB* audioDB, EventListener* eventListener, 
        rapidjson::Document& template_json, int current_actor_id);
    Actor UseTemplate(std::string template_name);

private:
    TemplateDB() {}  // Private constructor prevents external instantiation
    std::unordered_map<uint64_t, Actor> actor_templates;
};