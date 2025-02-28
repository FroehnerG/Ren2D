#include "Renderer.h"

Renderer::Renderer(SDL_Window* window, int r, int g, int b)
    : clear_color_r(r), clear_color_g(g), clear_color_b(b) {
    if (!window) {
        std::cerr << "Error: Invalid SDL_Window pointer!" << std::endl;
        sdl_renderer = nullptr;
        return;
    }

    sdl_renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdl_renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    }
}

SDL_Renderer* Renderer::GetRenderer()
{
    return sdl_renderer;
}

void Renderer::SetRenderer(SDL_Window* window)
{
	sdl_renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
}

void Renderer::SetClearColor(int r, int g, int b)
{
    clear_color_r = r;
    clear_color_g = g;
    clear_color_b = b;
}

void Renderer::Render()
{
    bool is_running = true;  // Flag to control game loop

    while (is_running) {
        // Set the background color
        SDL_SetRenderDrawColor(sdl_renderer, clear_color_r, clear_color_g, clear_color_b, 255);
        SDL_RenderClear(sdl_renderer);

        // Process events
        SDL_Event e;
        while (Helper::SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                is_running = false; // Instead of exit(0), set the flag to false
            }
        }

        // Present the frame (finish rendering)
        Helper::SDL_RenderPresent(sdl_renderer);
    }
}
