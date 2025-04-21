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

void Renderer::SetCamEaseFactor(float cam_ease_factor_in)
{
    cam_ease_factor = cam_ease_factor_in;
}

void Renderer::SetCameraPosition(Actor* player) {
    if (player) {
        camera_position = player->position + cam_offset;
    }
    else {
        camera_position = cam_offset; 
    }
}

void Renderer::SetXFlipOnMovement(bool x_flip_on_movement)
{
    x_scale_actor_flipping_on_movement = x_flip_on_movement;
}

bool Renderer::GetXFlipOnMovement()
{
    return x_scale_actor_flipping_on_movement;
}

void Renderer::Render(std::multimap<RenderKey, const Actor*>* sorted_actors, std::pair<Actor*, std::string>* dialogue, Actor* player, int& x_resolution, 
    int& y_resolution, SDL_Texture* hp_image, std::optional<int> health, int& score)
{
    SDL_RenderSetScale(sdl_renderer, zoom_factor, zoom_factor);

    // Set background color
    SDL_SetRenderDrawColor(sdl_renderer, clear_color_r, clear_color_g, clear_color_b, 255);
    SDL_RenderClear(sdl_renderer);

    if (player) {
        camera_position = glm::mix(camera_position, player->position + cam_offset, cam_ease_factor);
    }

    glm::vec2 extra_view_offset = glm::vec2(0, -glm::abs(glm::sin(Helper::GetFrameNumber() * 0.15f)) * 10.0f);

    for (const auto& actor : *sorted_actors) {
        if (actor.second->view_image == nullptr) {
            continue;
        }

        SDL_Texture* actor_view_image = actor.second->view_image;

        if (actor.second->show_view_image_back) {
            actor_view_image = actor.second->view_image_back;
        }

        if (actor.second->show_view_image_damage) {
            actor_view_image = actor.second->view_image_damage;
        }

        // Get image width and height
        float img_width = 0, img_height = 0;
        Helper::SDL_QueryTexture(actor_view_image, &img_width, &img_height);

        glm::vec2 extra_view_offset(0.0f);

        if (actor.second->movement_bounce_enabled && actor.second->velocity != glm::vec2(0.0f)) {
            extra_view_offset = glm::vec2(0, -glm::abs(glm::sin(Helper::GetFrameNumber() * 0.15f)) * 10.0f);
        }

        // Adjust for zoom: divide camera offset by zoom
        float screen_x = (x_resolution / 2.0f / zoom_factor) + ((actor.second->position.x - camera_position.x) * scale_units);
        float screen_y = (y_resolution / 2.0f / zoom_factor) + ((actor.second->position.y - camera_position.y) * scale_units);

        screen_x += extra_view_offset.x;
        screen_y += extra_view_offset.y;

        // Use the already defined view_pivot_offset
        SDL_FPoint pivot = { actor.second->view_pivot_offset.x, actor.second->view_pivot_offset.y };

        // Define the destination rectangle
        SDL_FRect dstrect;
        dstrect.x = screen_x - pivot.x * glm::abs(actor.second->transform_scale.x);  // Offset by pivot
        dstrect.y = screen_y - pivot.y * glm::abs(actor.second->transform_scale.y);
        dstrect.w = img_width * glm::abs(actor.second->transform_scale.x);
        dstrect.h = img_height * glm::abs(actor.second->transform_scale.y);

        // Determine Flip State (based on transform_scale)
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (actor.second->transform_scale.x < 0) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
        if (actor.second->transform_scale.y < 0) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);

        // Render the actor with rotation, scaling, and flipping
        Helper::SDL_RenderCopyEx(
            actor.second->id,
            actor.second->actor_name,
            sdl_renderer,
            actor_view_image,
            nullptr,  // Full texture 
            &dstrect,
            actor.second->transform_rotation_degrees,
            &pivot,
            flip
        );
    }

    //RenderDebugColliders(sorted_actors, x_resolution, y_resolution, false); // true = also render triggers
    SDL_RenderSetScale(sdl_renderer, 1.0f, 1.0f);

    if (health.has_value()) {
        std::string score_text = "score : " + std::to_string(score);

        RenderHealth(hp_image, *health, x_resolution, y_resolution);
        DrawText(score_text, 16, { 255, 255, 255, 255 }, 5, 5);
        RenderDialogue(dialogue, x_resolution, y_resolution);
    }

    // Present the frame (finish rendering)
    Helper::SDL_RenderPresent(sdl_renderer);
    dialogue->second.clear();
}

void Renderer::RenderDebugColliders(std::multimap<RenderKey, const Actor*>* sorted_actors, int& x_resolution, int& y_resolution, bool show_triggers)
{
    SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);

    for (const auto& actor : *sorted_actors) {
        // Choose which box to draw
        bool draw_collider = actor.second->box_collider_width > 0 && actor.second->box_collider_height > 0;
        bool draw_trigger = show_triggers && actor.second->box_trigger_width > 0 && actor.second->box_trigger_height > 0;

        if (!draw_collider && !draw_trigger) continue;

        float scale_units = 100.0f;

        // Convert world position to screen
        float screen_x = (x_resolution / 2.0f / zoom_factor) + ((actor.second->position.x - camera_position.x) * scale_units);
        float screen_y = (y_resolution / 2.0f / zoom_factor) + ((actor.second->position.y - camera_position.y) * scale_units);

        // Draw collider (red)
        if (draw_collider) {
            float w = actor.second->box_collider_width * glm::abs(actor.second->transform_scale.x) * scale_units;
            float h = actor.second->box_collider_height * glm::abs(actor.second->transform_scale.y) * scale_units;

            SDL_FRect rect;
            rect.x = screen_x - w / 2.0f;
            rect.y = screen_y - h / 2.0f;
            rect.w = w;
            rect.h = h;

            SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 180);  // Red box
            SDL_RenderDrawRectF(sdl_renderer, &rect);
        }

        // Draw trigger (green)
        if (draw_trigger) {
            float w = actor.second->box_trigger_width * glm::abs(actor.second->transform_scale.x) * scale_units;
            float h = actor.second->box_trigger_height * glm::abs(actor.second->transform_scale.y) * scale_units;

            SDL_FRect rect;
            rect.x = screen_x - w / 2.0f;
            rect.y = screen_y - h / 2.0f;
            rect.w = w;
            rect.h = h;

            SDL_SetRenderDrawColor(sdl_renderer, 0, 255, 0, 180);  // Green box
            SDL_RenderDrawRectF(sdl_renderer, &rect);
        }
    }

    SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_NONE);
}

void Renderer::RenderDialogue(std::pair<Actor*, std::string>* dialogue, int x_resolution, int y_resolution)
{
    if (!dialogue || !dialogue->first || dialogue->second.empty()) return;

    const Actor* speaker = dialogue->first;
    const std::string& text = dialogue->second;

    const int box_height = 120;
    const int box_padding = 20;
    const int portrait_size = 96;
    const float vertical_offset = 40.0f;

    const float box_width = static_cast<float>(x_resolution);

    // Draw translucent black box
    SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 200);

    SDL_FRect background_box = {
        0.5f * (x_resolution - box_width),                                // Centered horizontally
        static_cast<float>(y_resolution - box_height - vertical_offset), // Slightly above bottom
        box_width,
        static_cast<float>(box_height)
    };
    SDL_RenderFillRectF(sdl_renderer, &background_box);

    // Determine portrait texture (event-specific or default)
    SDL_Texture* portrait = nullptr;
    for (const auto& [event_name, message] : speaker->dialogue.event_to_text) {
        if (message == text) {
            auto it = speaker->dialogue.event_to_portrait_texture.find(event_name);
            if (it != speaker->dialogue.event_to_portrait_texture.end()) {
                portrait = it->second;
            }
            break;
        }
    }
    if (!portrait) {
        portrait = speaker->current_portrait;
    }

    float box_top = background_box.y;

    // Draw portrait if available
    if (portrait) {
        SDL_FRect portrait_rect = {
            static_cast<float>(box_padding),
            box_top + (box_height - portrait_size) / 2.0f,
            static_cast<float>(portrait_size),
            static_cast<float>(portrait_size)
        };
        SDL_RenderCopyF(sdl_renderer, portrait, nullptr, &portrait_rect);
    }

    // Draw text to the right of the portrait
    int text_x = box_padding + portrait_size + 20;
    int text_y = static_cast<int>(box_top + box_padding);

    DrawText(text, 20, { 255, 255, 255, 255 }, text_x, text_y);

    SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_NONE);

    dialogue->second.clear();  // Clear string content after displaying
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