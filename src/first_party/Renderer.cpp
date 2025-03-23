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

void Renderer::SetCamOffset(glm::vec2 cam_offset_in)
{
    cam_offset = cam_offset_in;
}

void Renderer::SetZoomFactor(float zoom_factor_in)
{
    zoom_factor = zoom_factor_in;
}


void Renderer::Render(vector<Actor>* actors, vector<string>* dialogue, Actor* player, int& x_resolution, int& y_resolution, 
    SDL_Texture* hp_image, std::optional<int> health, int& score)
{
    SDL_RenderSetScale(sdl_renderer, zoom_factor, zoom_factor);

    // Set background color
    SDL_SetRenderDrawColor(sdl_renderer, clear_color_r, clear_color_g, clear_color_b, 255);
    SDL_RenderClear(sdl_renderer);

    glm::vec2 camera_position = glm::vec2(0, 0);

    if (player) {
        camera_position = player->position;
    }

    // Create a sorted copy of actor pointers (without modifying the original vector)
    std::vector<const Actor*> sorted_actors;
    sorted_actors.reserve(actors->size()); // Avoids multiple reallocations

    for (const auto& actor : *actors) {
        sorted_actors.push_back(&actor);
    }
    
    // Sort using render_order if available, otherwise use position.y
    std::sort(sorted_actors.begin(), sorted_actors.end(), [](const Actor* a, const Actor* b) {
        float a_sort_value = a->render_order.value_or(a->position.y);
        float b_sort_value = b->render_order.value_or(b->position.y);
        return a_sort_value < b_sort_value;
    });

    for (const auto* actor : sorted_actors) {
        if (actor->view_image == nullptr) {
            continue;
        }

        // Get image width and height
        float img_width = 0, img_height = 0;
        Helper::SDL_QueryTexture(actor->view_image, &img_width, &img_height);

        float scale_units = 100.0f;  // your world units to pixel scale

        // Adjust for zoom: divide camera offset by zoom
        float screen_x = (x_resolution / 2.0f / zoom_factor) + ((actor->position.x - camera_position.x - cam_offset.x) * scale_units);
        float screen_y = (y_resolution / 2.0f / zoom_factor) + ((actor->position.y - camera_position.y - cam_offset.y) * scale_units);

        // Use the already defined view_pivot_offset
        SDL_FPoint pivot = { actor->view_pivot_offset.x, actor->view_pivot_offset.y };

        // Define the destination rectangle
        SDL_FRect dstrect;
        dstrect.x = screen_x - pivot.x * glm::abs(actor->transform_scale.x);  // Offset by pivot
        dstrect.y = screen_y - pivot.y * glm::abs(actor->transform_scale.y);
        dstrect.w = img_width * glm::abs(actor->transform_scale.x);
        dstrect.h = img_height * glm::abs(actor->transform_scale.y);

        // Determine Flip State (based on transform_scale)
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (actor->transform_scale.x < 0) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
        if (actor->transform_scale.y < 0) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);

        // Render the actor with rotation, scaling, and flipping
        Helper::SDL_RenderCopyEx(
            actor->id,
            actor->actor_name,
            sdl_renderer,
            actor->view_image,
            nullptr,  // Full texture 
            &dstrect,
            actor->transform_rotation_degrees,
            &pivot,
            flip
        );
    }

    SDL_RenderSetScale(sdl_renderer, 1.0f, 1.0f);

    if (health != std::nullopt) {
        std::string score_text = "score : " + std::to_string(score);

        RenderHealth(hp_image, *health, x_resolution, y_resolution);
        DrawText(score_text, 16, { 255, 255, 255, 255 }, 5, 5);
        RenderDialogue(dialogue, y_resolution);
    }

    // Present the frame (finish rendering)
    Helper::SDL_RenderPresent(sdl_renderer);
}

void Renderer::RenderDialogue(vector<string>* dialogue, int y_resolution)
{
    // Render dialogue messages to screen (bottom-left)
    int m = dialogue->size();
    for (int i = 0; i < m; ++i) {
        int x = 25;
        int y = y_resolution - 50 - (m - 1 - i) * 50;

        DrawText(dialogue->at(i), 16, { 255, 255, 255, 255 }, x, y);  // white text, size 24
    }
}

void Renderer::RenderEnd(SDL_Texture* game_over_image)
{
    SDL_SetRenderDrawColor(sdl_renderer, clear_color_r, clear_color_g, clear_color_b, 255);
    SDL_RenderClear(sdl_renderer);
    Helper::SDL_RenderCopy(sdl_renderer, game_over_image, nullptr, nullptr);
    Helper::SDL_RenderPresent(sdl_renderer);
}

void Renderer::RenderIntro(ImageDB* imageDB, TextDB* textDB, int& y_resolution)
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

void Renderer::RenderHealth(SDL_Texture* hp_image, int& health, int& x_resolution, int& y_resolution)
{    // Get image width and height
    float img_width = 0, img_height = 0;
    Helper::SDL_QueryTexture(hp_image, &img_width, &img_height);

    for (int i = 0; i < health; i++) {
        // Convert actor position from in-game units to screen pixels, centered 
        int screen_x = 5 + (i * (img_width + 5));
        int screen_y = 25;

        // Define the destination rectangle
        SDL_FRect dstrect;
        dstrect.x = screen_x;
        dstrect.y = screen_y;
        dstrect.w = img_width;
        dstrect.h = img_height;

        // Render the actor with rotation, scaling, and flipping
        Helper::SDL_RenderCopyEx(
            -1,
            "heart",
            sdl_renderer,
            hp_image,
            nullptr,  // Full texture 
            &dstrect,
            0,
            nullptr,
            SDL_FLIP_NONE
        );
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