#include "Actor.h"
#include "Helper.h"

void Actor::ParseActorFromJson(SDL_Renderer* renderer, ImageDB* imageDB, rapidjson::Value& actor_json, int current_actor_id)
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

    if (actor_json.HasMember("vel_x")) {
        velocity.x = actor_json["vel_x"].GetInt();
    }

    if (actor_json.HasMember("vel_y")) {
        velocity.y = actor_json["vel_y"].GetInt();
    }

    if (actor_json.HasMember("transform_rotation_degrees")) {
        transform_rotation_degrees = actor_json["transform_rotation_degrees"].GetFloat();
    }

    if (actor_json.HasMember("blocking")) {
        blocking = actor_json["blocking"].GetBool();
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

        // TODO: Set default view_pivot_offset based on image height and width
        float width, height;
        Helper::SDL_QueryTexture(imageDB->GetActorTextureById(current_actor_id), &width, &height);

        view_pivot_offset.x = width * 0.5f;
        view_pivot_offset.y = height * 0.5f;
    }


    if (actor_json.HasMember("view_pivot_offset_x")) {
        view_pivot_offset.x = actor_json["view_pivot_offset_x"].GetFloat();
    }

    if (actor_json.HasMember("view_pivot_offset_y")) {
        view_pivot_offset.y = actor_json["view_pivot_offset_y"].GetFloat();
    }
}
