#include <iostream>
#include <filesystem>
#include <cstdlib> // For exit(0)
#include "ImageDB.h"
#include "EngineUtils.h"
#include "Helper.h"

namespace fs = std::filesystem;

void ImageDB::LoadImages(rapidjson::Value& image_json, SDL_Renderer* renderer, bool is_intro, std::string image_name, int actor_id)
{
	if (is_intro && image_json.HasMember("intro_image")) {
		for (const auto& image : image_json["intro_image"].GetArray()) {
            std::string intro_image_name = image.GetString();
            std::string intro_image_path = "resources/images/" + intro_image_name + ".png";

            if (!fs::exists(intro_image_path)) {
                std::cout << "error: missing image " << intro_image_name;
                exit(0);
            }

            SDL_Texture* texture = IMG_LoadTexture(renderer, intro_image_path.c_str());
            if (!texture) {
                std::cout << "error: failed to load texture " << intro_image_name
                    << " (" << IMG_GetError() << ")" << std::endl;
                exit(0);
            }

            intro_images.push_back(texture);
		}

		return;
	}

    // Load actor view image
    std::string image_path = "resources/images/" + image_name + ".png";

    if (!fs::exists(image_path)) {
        std::cout << "error: missing image " << image_name;
        exit(0);
    }

    SDL_Texture* texture = IMG_LoadTexture(renderer, image_path.c_str());
    if (!texture) {
        std::cout << "error: failed to load texture " << image_name
            << " (" << IMG_GetError() << ")" << std::endl;
        exit(0);
    }

    actor_textures[actor_id] = texture;
}

SDL_Texture* ImageDB::GetCurrentIntroImage() {
    return intro_images[current_intro_index];
}

SDL_Texture* ImageDB::GetLastIntroImage()
{
    return intro_images.back();
}

SDL_Texture* ImageDB::GetActorTextureById(int actor_id)
{
    return actor_textures[actor_id];
}

void ImageDB::AdvanceIntro()
{
    if (IsIntroPlaying()) {
        current_intro_index++; // Move to the next image
    }
}

void ImageDB::CacheImage(SDL_Texture* texture, std::string image_name)
{
    uint64_t image_name_hash = EngineUtils::HashString(image_name);

    textures[image_name_hash] = texture;
}
