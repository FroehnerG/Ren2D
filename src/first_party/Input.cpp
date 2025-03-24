#include "Input.h"

void Input::Init() {
    // Clear all input state
    keyboard_states.clear();
    just_became_down_scancodes.clear();
    just_became_up_scancodes.clear();
}

void Input::ProcessEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        SDL_Scancode scancode = e.key.keysym.scancode;

        // If it was up before, mark as just became down
        if (keyboard_states[scancode] == INPUT_STATE_UP || keyboard_states[scancode] == INPUT_STATE_JUST_BECAME_UP) {
            keyboard_states[scancode] = INPUT_STATE_JUST_BECAME_DOWN;
            just_became_down_scancodes.push_back(scancode);
        }
    }
    else if (e.type == SDL_KEYUP) {
        SDL_Scancode scancode = e.key.keysym.scancode;

        keyboard_states[scancode] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_scancodes.push_back(scancode);
    }
}

void Input::LateUpdate() {
    // Transition all just_became_down keys to down
    for (SDL_Scancode scancode : just_became_down_scancodes) {
        keyboard_states[scancode] = INPUT_STATE_DOWN;
    }
    just_became_down_scancodes.clear();

    // Transition all just_became_up keys to up
    for (SDL_Scancode scancode : just_became_up_scancodes) {
        keyboard_states[scancode] = INPUT_STATE_UP;
    }
    just_became_up_scancodes.clear();
}

bool Input::GetKey(SDL_Scancode keycode) {
    INPUT_STATE state = keyboard_states[keycode];
    return (state == INPUT_STATE_DOWN || state == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetKeyDown(SDL_Scancode keycode) {
    return keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyUp(SDL_Scancode keycode) {
    return keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_UP;
}
