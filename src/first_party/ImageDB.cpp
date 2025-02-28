#include <iostream>
#include <filesystem>
#include <cstdlib> // For exit(0)
#include "ImageDB.h"
#include "EngineUtils.h"
#include "Helper.h"

namespace fs = std::filesystem;

void ImageDB::LoadImages(rapidjson::Document& game_config, SDL_Renderer* renderer, std::string image_type)
{
	if (image_type == "intro_image" && game_config.HasMember("intro_image")) {
		for (const auto& image : game_config["intro_image"].GetArray()) {
            std::string image_name = image.GetString();
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

            intro_images.push_back(texture);
		}

		return;
	}
}

void ImageDB::RenderIntroImage(SDL_Renderer* renderer)
{
    if (IsIntroPlaying() && intro_images[current_intro_index]) {
        Helper::SDL_RenderCopy(renderer, intro_images[current_intro_index], nullptr, nullptr);
    }
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
