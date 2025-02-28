#pragma once
#include <string>
#include "EngineUtils.h"
#include "SDL2/SDL.h"
#include "Helper.h"

class Renderer {
public:
    // Constructor with RGB color initialization
    Renderer(SDL_Window* window, int r = 255, int g = 255, int b = 255);
    Renderer() {}

    SDL_Renderer* GetRenderer();
	void SetRenderer(SDL_Window* window);
    void SetClearColor(int r, int g, int b);
	void Render();

private:
	SDL_Renderer* sdl_renderer = nullptr;
    int clear_color_r = 255;
    int clear_color_g = 255;
    int clear_color_b = 255;
};