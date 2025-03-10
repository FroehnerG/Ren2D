#include <iostream>
#include <filesystem>
#include <cstdlib> // For exit(0)
#include "TextDB.h"

namespace fs = std::filesystem;

void TextDB::LoadText(rapidjson::Document& game_config, ImageDB* images, bool is_intro_text)
{
	if (is_intro_text) {
		if (!game_config.HasMember("intro_image")) {
			return;
		}

		if (!game_config.HasMember("intro_text")) {
			has_intro_text = false;
			return;
		}
	}

	if (!game_config.HasMember("font")) {
		std::cout << "error: text render failed. No font configured";
		exit(0);
	}

	std::string font_name = game_config["font"].GetString();

	std::string font_path = "resources/fonts/" + font_name + ".ttf";

	if (!fs::exists(font_path)) {
		std::cout << "error: font " << font_name << " missing";
		exit(0);
	}

	text_font = TTF_OpenFont(font_path.c_str(), 16);

	if (!text_font) {
		exit(0);
	}

	for (const auto& text : game_config["intro_text"].GetArray()) {
		intro_text.push_back(text.GetString());
	}
	
	if (intro_text.size() == 0) {
		has_intro_text = false;
	}
}

void TextDB::AdvanceIntroText()
{
	if (IsIntroPlaying()) {
		current_text_intro_index++; // Move to the next image
	}
}

TTF_Font* TextDB::GetTextFont()
{
	return text_font;
}

std::string TextDB::GetCurrentIntroText() {
	return intro_text[current_text_intro_index];
}

std::string TextDB::GetLastIntroText()
{
	return intro_text.back();
}
