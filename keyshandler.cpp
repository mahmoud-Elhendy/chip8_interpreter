#include "keyshandler.h"

KeysHandler::KeysHandler(bool (&keys)[KEYS_COUNT]): keys(keys) {}

/*
1, 2, 3, C → 1, 2, 3, 4 
4, 5, 6, D → Q, W, E, R.
7, 8, 9, E → A, S, D, F.
A, 0, B, F → Z, X, C, V.
*/
Action KeysHandler::handle_keys() {  
    
    SDL_Event event;
    Action action = NO_ACTION;

    while (SDL_PollEvent(&event)) {
        if (event.type != SDL_QUIT && event.type != SDL_KEYDOWN && event.type != SDL_KEYUP)
            continue;

        if (event.type == SDL_QUIT) return EXIT;

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) return EXIT;

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)  action = RESET;

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool isPressed = (event.type == SDL_KEYDOWN);
            switch (event.key.keysym.sym) {
                case SDLK_1: keys[0x1] = isPressed; break;
                case SDLK_2: keys[0x2] = isPressed; break;
                case SDLK_3: keys[0x3] = isPressed; break;
                case SDLK_4: keys[0xC] = isPressed; break;

                case SDLK_q: keys[0x4] = isPressed; break;
                case SDLK_w: keys[0x5] = isPressed; break;
                case SDLK_e: keys[0x6] = isPressed; break;
                case SDLK_r: keys[0xD] = isPressed; break;

                case SDLK_a: keys[0x7] = isPressed; break;
                case SDLK_s: keys[0x8] = isPressed; break;
                case SDLK_d: keys[0x9] = isPressed; break;
                case SDLK_f: keys[0xE] = isPressed; break;

                case SDLK_z: keys[0xA] = isPressed; break;
                case SDLK_x: keys[0x0] = isPressed; break;
                case SDLK_c: keys[0xB] = isPressed; break;
                case SDLK_v: keys[0xF] = isPressed; break;
                
                default: break;
            }
        }
    }

    return action;
}

