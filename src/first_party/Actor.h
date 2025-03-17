#pragma once

#include <string>
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
    glm::ivec2 position = glm::ivec2(0, 0);
    glm::ivec2 velocity = glm::ivec2(0, 0);
    glm::vec2 transform_scale = glm::vec2(1.0f, 1.0f);
    float transform_rotation_degrees = 0.0f;
    glm::vec2 view_pivot_offset = glm::vec2(0.0f, 0.0f);
    bool blocking = false;
    std::string nearby_dialogue = "";
    std::string contact_dialogue = "";

    void ParseActorFromJson(SDL_Renderer* renderer, ImageDB* imageDB, rapidjson::Value& actor_json, int current_actor_id);
};
