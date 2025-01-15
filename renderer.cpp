#include "renderer.h"

bool Renderer::init_renderer(){ 
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        "CHIP-8",                             
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        CHIP8_WIDTH* SCALER,
        CHIP8_HEIGHT* SCALER,                    
        SDL_WINDOW_SHOWN                    
    );

    if (!window) {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Create SDL texture for rendering the CHIP-8 display
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (!texture) {
        std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    return true;
}

void Renderer::clean_renderer(){
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    SDL_Quit(); 
}

void Renderer::render_frame(uint8_t (&display) [CHIP8_WIDTH * CHIP8_HEIGHT]){
    uint32_t pixels[CHIP8_WIDTH * CHIP8_HEIGHT];
    for (int i = 0; i < CHIP8_WIDTH * CHIP8_HEIGHT; i++) {
        pixels[i] = (display[i] == 1) ? 0xFFFFFFFF : 0x00000000;
    }

    SDL_UpdateTexture(texture, nullptr, pixels, CHIP8_WIDTH * sizeof(uint32_t));

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

Renderer::~Renderer(){
    clean_renderer();
}