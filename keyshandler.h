#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>

constexpr uint8_t KEYS_COUNT = 16;

enum Action {
    EXIT,
    RESET,
    NO_ACTION
};

class KeysHandler {
public:
    KeysHandler(bool (&keys)[KEYS_COUNT]);
    Action handle_keys();
    ~KeysHandler() = default;
private:
    bool (&keys)[KEYS_COUNT];       
};