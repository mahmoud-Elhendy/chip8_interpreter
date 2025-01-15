#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>

#include "renderer.h"
#include "keyshandler.h"
#include "chip8.h"

constexpr int64_t timer_interval_ms = 1000 / 60; // 16.67 ms

int main(int argc, char* argv[]){
    
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_rom>" << std::endl;
        return 1;
    }
   
    Renderer renderer;
    if (!renderer.init_renderer())
        return 1;
    Chip8 chip8;
    if(!chip8.loadROM(argv[1]))
        return 1;

    KeysHandler keyshandler(chip8.keypad);
    auto last_time = std::chrono::high_resolution_clock::now();
    
    // Main game loop
    while (true) {
        //handle keys input
        Action action = keyshandler.handle_keys();
        if (action == EXIT) break;
        if(action == RESET) {
            chip8.reset();
        }
        //emulate cycle
        chip8.emulateCycle();
        
        //update timers
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_time);

        if (elapsed_time.count() >= timer_interval_ms) {
            chip8.update_timers();
            last_time = current_time;    
        }
        //update screen    
        if(chip8.draw_flag){
                chip8.draw_flag = false;
                renderer.render_frame(chip8.display);      
        }
        //8 instructions per 16ms
        std::this_thread::sleep_for(std::chrono::milliseconds(16/8));
    }        

    return 0;
}