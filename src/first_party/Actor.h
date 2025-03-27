#pragma once

#include <string>
#include <optional>
#include "glm/glm.hpp"
#include "SDL2/SDL.h"
#include "rapidjson/document.h"
#include "ImageDB.h"

class Actor {
public:
    Actor() {}

    int id = -1;
    std::string actor_name = "";
    SDL_Texture* view_image = nullptr;
    SDL_Texture* view_image_back = nullptr;
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
    glm::vec2 transform_scale = glm::vec2(1.0f, 1.0f);
    float transform_rotation_degrees = 0.0f;
    glm::vec2 view_pivot_offset = glm::vec2(0.0f, 0.0f);
    bool blocking = false;
    bool x_scale_actor_flipping_on_movement = false;
    bool movement_bounce_enabled = false;
    bool direction_changed = true;
    bool show_view_image_back = false;
    std::string nearby_dialogue = "";
    std::string contact_dialogue = "";
    std::optional<float> render_order = std::nullopt;

    void ParseActorFromJson(SDL_Renderer* renderer, ImageDB* imageDB, rapidjson::Value& actor_json, int current_actor_id);
};
