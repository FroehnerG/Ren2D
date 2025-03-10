#pragma once
#include <string>
#include <vector>
#include "SDL2/SDL.h"
#include "rapidjson/document.h"
#include "SDL2_ttf/SDL_ttf.h"
#include "ImageDB.h"

class TextDB {
public:
	void LoadText(rapidjson::Document& game_config, ImageDB* images, bool is_intro_text);
	void AdvanceIntroText();
	TTF_Font* GetTextFont();
	std::string GetCurrentIntroText();
	std::string GetLastIntroText();

	bool IsIntroPlaying() const { return current_text_intro_index < intro_text.size(); }
private:
	size_t current_text_intro_index = 0;
	std::vector<std::string> intro_text;
	TTF_Font* text_font = nullptr;
};