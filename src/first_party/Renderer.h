#pragma once
#include <string>
#include <utility>
#include <optional>
#include "EngineUtils.h"
#include "SDL2/SDL.h"
#include "Helper.h"
#include "ImageDB.h"
#include "TextDB.h"
#include "SceneDB.h"

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
    void SetCamOffset(glm::vec2 cam_offset_in);
    void SetZoomFactor(float zoom_factor_in);
    void SetCamEaseFactor(float cam_ease_factor_in);
    void SetCameraPosition(Actor* player);
    void SetXFlipOnMovement(bool x_flip_on_movement);
    bool GetXFlipOnMovement();
	void Render(std::multimap<RenderKey, const Actor*>* sorted_actors, std::pair<Actor*, std::string>* dialogue, Actor* player,
        int& x_resolution, int& y_resolution, SDL_Texture* hp_image, std::optional<int> health, int& score);
    void RenderDebugColliders(std::multimap<RenderKey, const Actor*>* sorted_actors, int& x_resolution, int& y_resolution, bool show_triggers);
    void RenderDialogue(std::pair<Actor*, std::string>* dialogue, int x_resolution, int y_resolution);
    void RenderEnd(SDL_Texture* game_over_image);
    // New methods for rendering images
    void RenderIntro(ImageDB* imageDB, TextDB* textDB, int& y_resolution);
    void RenderHealth(SDL_Texture* hp_image, int& health, int& x_resolution, int& y_resolution);
    void DrawText(const std::string& text_content, int font_size, SDL_Color font_color, int x, int y);

private:
	SDL_Renderer* sdl_renderer = nullptr;
    TTF_Font* text_font = nullptr;
    glm::vec2 camera_position = glm::vec2(0.0f, 0.0f);
    glm::vec2 cam_offset = glm::vec2(0.0f, 0.0f);
    float scale_units = 100.0f;
    float zoom_factor = 1.0f;
    float cam_ease_factor = 1.0f;
    int clear_color_r = 255;
    int clear_color_g = 255;
    int clear_color_b = 255;
    bool x_scale_actor_flipping_on_movement = false;
};