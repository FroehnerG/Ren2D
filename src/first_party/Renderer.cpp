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

int Renderer::GetColor(std::string color)
{
    if (color == "red") {
        return clear_color_r;
    }
    else if (color == "green") {
        return clear_color_g;
    }
    else {
        return clear_color_b;
    }
}

void Renderer::SetFont(TextDB* textDB) {
    text_font = textDB->GetTextFont();
}

void Renderer::SetRenderer(SDL_Window* window)
{
	sdl_renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void Renderer::SetClearColor(int r, int g, int b)
{
    clear_color_r = r;
    clear_color_g = g;
    clear_color_b = b;
}

void Renderer::Render()
{
    // Set background color
    SDL_SetRenderDrawColor(sdl_renderer, clear_color_r, clear_color_g, clear_color_b, 255);
    SDL_RenderClear(sdl_renderer);

    // Present the frame (finish rendering)
    Helper::SDL_RenderPresent(sdl_renderer);
}

void Renderer::RenderIntro(ImageDB* imageDB, TextDB* textDB, int y_resolution)
{
    if (imageDB->IsIntroPlaying()) {
        Helper::SDL_RenderCopy(sdl_renderer, imageDB->GetCurrentIntroImage(), nullptr, nullptr);
        DrawText(textDB->GetCurrentIntroText(), 16, {255, 255, 255, 255}, 25, (y_resolution - 50));
    }
}

void Renderer::DrawText(const std::string& text_content, int font_size, SDL_Color font_color, int x, int y)
{
    SDL_Surface* textSurface = TTF_RenderText_Solid(text_font, text_content.c_str(), font_color);

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(sdl_renderer, textSurface);
    SDL_FreeSurface(textSurface);

    // Define the area to draw the text with a fixed height of 16
    SDL_FRect renderQuad = { x, y, 0, font_size };

    // Render the text
    Helper::SDL_RenderCopy(sdl_renderer, textTexture, nullptr, &renderQuad);

    // Clean up the texture
    SDL_DestroyTexture(textTexture);
}