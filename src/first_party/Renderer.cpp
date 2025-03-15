#include "Renderer.h"

Renderer::Renderer(SDL_Window* window, int r, int g, int b)
    : clear_color_r(r), clear_color_g(g), clear_color_b(b) {
    if (!window) {
        std::cout << "Error: Invalid SDL_Window pointer!" << std::endl;
        sdl_renderer = nullptr;
        return;
    }

    sdl_renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdl_renderer) {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
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

void Renderer::Render(vector<Actor>* actors)
{
    // Set background color
    SDL_SetRenderDrawColor(sdl_renderer, clear_color_r, clear_color_g, clear_color_b, 255);
    SDL_RenderClear(sdl_renderer);

    for (const auto& actor : *actors) {
        if (actor.view_image == nullptr) {
            continue;
        }

        // Get image width and height
        float img_width = 0, img_height = 0;
        Helper::SDL_QueryTexture(actor.view_image, &img_width, &img_height);

        // Convert actor position from in-game units to screen pixels
        int screen_x = actor.position.x * 100;
        int screen_y = actor.position.y * 100;

        // Use the already defined view_pivot_offset
        SDL_FPoint pivot = { actor.view_pivot_offset.x, actor.view_pivot_offset.y };

        // Define the destination rectangle
        SDL_FRect dstrect;
        dstrect.x = screen_x - pivot.x;  // Offset by pivot
        dstrect.y = screen_y - pivot.y;
        dstrect.w = img_width;
        dstrect.h = img_height;

        // Determine Flip State (based on transform_scale)
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (actor.transform_scale.x < 0) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
        if (actor.transform_scale.y < 0) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);

        // Render the actor with rotation, scaling, and flipping
        Helper::SDL_RenderCopyEx(
            actor.id,
            actor.actor_name,
            sdl_renderer,
            actor.view_image,
            nullptr,  // Full texture 
            &dstrect,
            actor.transform_rotation_degrees,
            &pivot,
            flip
        );
    }

    // Present the frame (finish rendering)
    Helper::SDL_RenderPresent(sdl_renderer);
}

void Renderer::RenderIntro(ImageDB* imageDB, TextDB* textDB, int y_resolution)
{
    if (imageDB->IsIntroPlaying() && textDB->IsIntroPlaying()) {
        Helper::SDL_RenderCopy(sdl_renderer, imageDB->GetCurrentIntroImage(), nullptr, nullptr);
        DrawText(textDB->GetCurrentIntroText(), 16, { 255, 255, 255, 255 }, 25, (y_resolution - 50));
    }
    else if (!imageDB->IsIntroPlaying() && textDB->IsIntroPlaying()) {
        Helper::SDL_RenderCopy(sdl_renderer, imageDB->GetLastIntroImage(), nullptr, nullptr);
        DrawText(textDB->GetCurrentIntroText(), 16, { 255, 255, 255, 255 }, 25, (y_resolution - 50));
    }
    else if (imageDB->IsIntroPlaying() && !textDB->IsIntroPlaying()) {
        Helper::SDL_RenderCopy(sdl_renderer, imageDB->GetCurrentIntroImage(), nullptr, nullptr);

        if (textDB->GetHasIntroText()) {
            DrawText(textDB->GetLastIntroText(), 16, { 255, 255, 255, 255 }, 25, (y_resolution - 50));
        }
    }
}

void Renderer::DrawText(const std::string& text_content, int font_size, SDL_Color font_color, int x, int y)
{
    SDL_Surface* textSurface = TTF_RenderText_Solid(text_font, text_content.c_str(), font_color);

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(sdl_renderer, textSurface);
    SDL_FreeSurface(textSurface);

    int text_width = 0, text_height = 0;  // Get both width and height dynamically
    TTF_SizeText(text_font, text_content.c_str(), &text_width, &text_height);  // Get dynamic width and height

    SDL_FRect renderQuad = { x, y, static_cast<float>(text_width), static_cast<float>(text_height) };

    // Render the text
    Helper::SDL_RenderCopy(sdl_renderer, textTexture, nullptr, &renderQuad);

    // Clean up the texture
    SDL_DestroyTexture(textTexture);
}