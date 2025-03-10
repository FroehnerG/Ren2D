#pragma once
#include <string>
#include "EngineUtils.h"
#include "SDL2/SDL.h"
#include "Helper.h"
#include "ImageDB.h"
#include "TextDB.h"

class Renderer {
public:
    // Constructor with RGB color initialization
    Renderer(SDL_Window* window, int r = 255, int g = 255, int b = 255);
    Renderer() {}

    SDL_Renderer* GetRenderer();
    int GetColor(std::string color);
    void SetFont(TextDB* textDB);
	void SetRenderer(SDL_Window* window);
    void SetClearColor(int r, int g, int b);
	void Render();
    // New methods for rendering images
    void RenderIntro(ImageDB* imageDB, TextDB* textDB, int y_resolution);
    void DrawText(const std::string& text_content, int font_size, SDL_Color font_color, int x, int y);

private:
	SDL_Renderer* sdl_renderer = nullptr;
    TTF_Font* text_font = nullptr;
    int clear_color_r = 255;
    int clear_color_g = 255;
    int clear_color_b = 255;
};