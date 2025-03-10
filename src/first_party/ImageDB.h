#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "EngineUtils.h"
#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"

class ImageDB {
public:
	void LoadImages(rapidjson::Document& game_config, SDL_Renderer* renderer, std::string image_name);
	void AdvanceIntro();
	void CacheImage(SDL_Texture* texture, std::string image_name);
	SDL_Texture* GetCurrentIntroImage();
	SDL_Texture* GetLastIntroImage();

	bool IsIntroPlaying() const { return current_intro_index < intro_images.size(); }
private:
	std::unordered_map<uint64_t, SDL_Texture*> textures;
	std::vector<SDL_Texture*> intro_images;
	size_t current_intro_index = 0;         // Index of current image being displayed
};