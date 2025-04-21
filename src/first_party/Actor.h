#pragma once

#include <string>
#include <optional>
#include <unordered_set>
#include "glm/glm.hpp"
#include "SDL2/SDL.h"
#include "rapidjson/document.h"
#include "ImageDB.h"
#include "AudioDB.h"
#include "EventListener.h"

struct DialogueComponent {
    std::vector<std::string> event_names;
    std::unordered_map<std::string, std::string> event_to_text;
    std::unordered_map<std::string, SDL_Texture*> event_to_portrait_texture;
};

class Actor {
public:
    Actor() {}

    int id = -1;
    std::string actor_name = "";
    SDL_Texture* view_image = nullptr;
    SDL_Texture* view_image_back = nullptr;
    SDL_Texture* view_image_damage = nullptr;
    SDL_Texture* default_portrait = nullptr;
    SDL_Texture* current_portrait = nullptr;
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
    glm::vec2 transform_scale = glm::vec2(1.0f, 1.0f);
    float transform_rotation_degrees = 0.0f;
    glm::vec2 view_pivot_offset = glm::vec2(0.0f, 0.0f);
    float box_collider_width = 0.0f;
    float box_collider_height = 0.0f;
    bool blocking = false;
    float box_trigger_width = 0.0f;
    float box_trigger_height = 0.0f;
    bool trigger = false;
    bool movement_bounce_enabled = false;
    bool direction_changed = true;
    bool show_view_image_back = false;
    bool show_view_image_damage = false;
    bool dialogue_cooldown = false;
    std::string contact_dialogue = "";
    std::string active_dialogue = "";
    std::optional<float> render_order = std::nullopt;
    std::unordered_set<Actor*> colliding_actors_this_frame;
    DialogueComponent dialogue;

    void ParseActorFromJson(SDL_Renderer* renderer, ImageDB* imageDB, AudioDB* audioDB, EventListener* eventListener,
        rapidjson::Value& actor_json, int current_actor_id);
    bool AreBoxesOverlapping(const Actor& other, bool is_trigger);
    void InsertCollidingActor(Actor* actor);
    void ClearCollidingActors();
};
