#include "Actor.h"
#include "Helper.h"

void Actor::ParseActorFromJson(SDL_Renderer* renderer, ImageDB* imageDB, AudioDB* audioDB, rapidjson::Value& actor_json, int current_actor_id)
{
    if (actor_json.HasMember("name")) {
        actor_name = actor_json["name"].GetString();
    }

    if (actor_json.HasMember("transform_position_x")) {
        position.x = actor_json["transform_position_x"].GetFloat();
    }

    if (actor_json.HasMember("transform_position_y")) {
        position.y = actor_json["transform_position_y"].GetFloat();
    }

    if (actor_json.HasMember("transform_scale_x")) {
        transform_scale.x = actor_json["transform_scale_x"].GetFloat();
    }

    if (actor_json.HasMember("transform_scale_y")) {
        transform_scale.y = actor_json["transform_scale_y"].GetFloat();
    }

    if (actor_json.HasMember("vel_x")) {
        velocity.x = actor_json["vel_x"].GetFloat();
    }

    if (actor_json.HasMember("vel_y")) {
        velocity.y = actor_json["vel_y"].GetFloat();
    }

    if (actor_json.HasMember("transform_rotation_degrees")) {
        transform_rotation_degrees = actor_json["transform_rotation_degrees"].GetFloat();
    }

    if (actor_json.HasMember("box_collider_width")) {
        box_collider_width = actor_json["box_collider_width"].GetFloat();
    }

    if (actor_json.HasMember("box_collider_height")) {
        box_collider_height = actor_json["box_collider_height"].GetFloat();
    }

    if (box_collider_width != 0.0f && box_collider_height != 0.0f) {
        blocking = true;
    }

    if (actor_json.HasMember("box_trigger_width")) {
        box_trigger_width = actor_json["box_trigger_width"].GetFloat();
    }

    if (actor_json.HasMember("box_trigger_height")) {
        box_trigger_height = actor_json["box_trigger_height"].GetFloat();
    }

    if (box_trigger_width != 0.0f && box_trigger_height != 0.0f) {
        trigger = true;
    }
    
    if (actor_json.HasMember("movement_bounce_enabled")) {
        movement_bounce_enabled = actor_json["movement_bounce_enabled"].GetBool();
    }

    if (actor_json.HasMember("nearby_dialogue")) {
        nearby_dialogue = actor_json["nearby_dialogue"].GetString();
    }

    if (actor_json.HasMember("contact_dialogue")) {
        contact_dialogue = actor_json["contact_dialogue"].GetString();
    }

    if (actor_json.HasMember("view_image")) {
        std::string image_name = actor_json["view_image"].GetString();
        imageDB->LoadImages(actor_json, renderer, false, image_name, current_actor_id);

        view_image = imageDB->GetActorTextureById(current_actor_id);

        // TODO: Set default view_pivot_offset based on image height and width
        float width, height;
        Helper::SDL_QueryTexture(imageDB->GetActorTextureById(current_actor_id), &width, &height);

        view_pivot_offset.x = width * 0.5f;
        view_pivot_offset.y = height * 0.5f;
    }

    if (actor_json.HasMember("view_image_back")) {
        std::string image_name = actor_json["view_image_back"].GetString();
        imageDB->LoadImages(actor_json, renderer, false, image_name, current_actor_id);

        view_image_back = imageDB->GetActorTextureById(current_actor_id);
    }

    if (actor_name == "player") {
        if (actor_json.HasMember("view_image_damage")) {
            std::string image_name = actor_json["view_image_damage"].GetString();
            imageDB->LoadImages(actor_json, renderer, false, image_name, current_actor_id);

            view_image_damage = imageDB->GetActorTextureById(current_actor_id);
        }

        if (actor_json.HasMember("damage_sfx")) {
            std::string sfx_name = actor_json["damage_sfx"].GetString();
            audioDB->LoadPlayerSFX("damage_sfx", sfx_name);
        }

        if (actor_json.HasMember("step_sfx")) {
            std::string sfx_name = actor_json["step_sfx"].GetString();
            audioDB->LoadPlayerSFX("step_sfx", sfx_name);
        }
    }
    else {
        if (actor_json.HasMember("view_image_attack")) {
            std::string image_name = actor_json["view_image_attack"].GetString();
            imageDB->LoadImages(actor_json, renderer, false, image_name, current_actor_id);

            view_image_damage = imageDB->GetActorTextureById(current_actor_id);
        }

        if (actor_json.HasMember("nearby_dialogue_sfx")) {
            std::string sfx_name = actor_json["nearby_dialogue_sfx"].GetString();
            audioDB->SetNearbyDialogueSFXByID(current_actor_id, sfx_name);
        }
    }

    if (actor_json.HasMember("view_pivot_offset_x")) {
        view_pivot_offset.x = actor_json["view_pivot_offset_x"].GetFloat();
    }

    if (actor_json.HasMember("view_pivot_offset_y")) {
        view_pivot_offset.y = actor_json["view_pivot_offset_y"].GetFloat();
    }

    if (actor_json.HasMember("render_order")) {
        render_order = actor_json["render_order"].GetFloat();
    }
}

bool Actor::AreBoxesOverlapping(const Actor& other, bool is_trigger)
{
    if (!is_trigger) {
        if (!blocking || !other.blocking) {
            return false; // can't check if either is missing
        }
    }
    else {
        if (!trigger || !other.trigger) {
            return false; // can't check if either is missing
        }
    }

    float a_half_w;
    float a_half_h;
    float b_half_w;
    float b_half_h;

    if (!is_trigger) {
        a_half_w = box_collider_width * glm::abs(transform_scale.x) / 2.0f;
        a_half_h = box_collider_height * glm::abs(transform_scale.y) / 2.0f;
        b_half_w = other.box_collider_width * glm::abs(other.transform_scale.x) / 2.0f;
        b_half_h = other.box_collider_height * glm::abs(other.transform_scale.y) / 2.0f;
    }
    else {
        a_half_w = box_trigger_width * glm::abs(transform_scale.x) / 2.0f;
        a_half_h = box_trigger_height * glm::abs(transform_scale.y) / 2.0f;
        b_half_w = other.box_trigger_width * glm::abs(other.transform_scale.x) / 2.0f;
        b_half_h = other.box_trigger_height * glm::abs(other.transform_scale.y) / 2.0f;
    }

    float a_left = position.x - a_half_w;
    float a_right = position.x + a_half_w;
    float a_top = position.y - a_half_h;
    float a_bottom = position.y + a_half_h;

    float b_left = other.position.x - b_half_w;
    float b_right = other.position.x + b_half_w;
    float b_top = other.position.y - b_half_h;
    float b_bottom = other.position.y + b_half_h;

    if (a_right <= b_left) return false;
    if (a_left >= b_right) return false;
    if (a_bottom <= b_top) return false;
    if (a_top >= b_bottom) return false;

    return true;  // All axes overlap
}

void Actor::InsertCollidingActor(Actor* actor)
{
    colliding_actors_this_frame.insert(actor);
}

void Actor::ClearCollidingActors()
{
    colliding_actors_this_frame.clear();
}


