#pragma once

#include <string>
#include "glm/glm.hpp"

struct Actor {
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
        : actor_name(actor_name), view(view), position(position), velocity(initial_velocity), blocking(blocking),
        nearby_dialogue(nearby_dialogue), contact_dialogue(contact_dialogue) {
        id = -1;
    }

    Actor() {}
};
