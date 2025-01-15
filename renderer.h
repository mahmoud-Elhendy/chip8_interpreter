#pragma once 

#include <iostream>
#include <SDL2/SDL.h>

constexpr uint8_t CHIP8_WIDTH = 64;
constexpr uint8_t CHIP8_HEIGHT = 32;
constexpr uint8_t SCALER = 10;

class Renderer {
public:
    Renderer() =default;
    ~Renderer();
    bool init_renderer();
    void clean_renderer();
    void render_frame(uint8_t (&display) [CHIP8_WIDTH * CHIP8_HEIGHT]);
private:
    SDL_Renderer* renderer = nullptr;
    SDL_Window* window = nullptr;
    SDL_Texture* texture = nullptr;
};