#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "EngineUtils.h"
#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"

class ImageDB {
public:
	void LoadImages(rapidjson::Value& image_json, SDL_Renderer* renderer, bool is_intro, std::string image_name, int actor_id);
	void AdvanceIntro();
	void CacheImage(SDL_Texture* texture, std::string image_name);
	SDL_Texture* GetCurrentIntroImage();
	SDL_Texture* GetLastIntroImage();
	SDL_Texture* GetActorTextureById(int actor_id);
	SDL_Texture* GetHPImage();

	bool IsIntroPlaying() const { return current_intro_index < intro_images.size(); }
private:
	std::unordered_map<uint64_t, SDL_Texture*> textures;
	std::unordered_map<int, SDL_Texture*> actor_textures; // Index by actor id
	std::vector<SDL_Texture*> intro_images;
	SDL_Texture* hp_image;
	size_t current_intro_index = 0;         // Index of current image being displayed
};