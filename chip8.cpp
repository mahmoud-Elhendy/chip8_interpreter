#include "chip8.h"

Chip8::Chip8():pc(start_addr),stack{}, sp(0),delay_timer(0),
sound_timer(0),draw_flag(false), I(0), V{}, keypad{false}, 
memory{}, display{},player{}{
    //load fontset into memory
    for (int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

/**
 * Resets the Chip8 emulator back to its initial state.
 *
 * This function resets all the Chip8 registers, timers, memory, and the display
 * to their initial states. It is called when the emulator is first started, and
 * can also be called at any time to restart the emulator.
 */
void Chip8::reset(){
    pc = start_addr;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    draw_flag = false;
    I = 0;
    for (int i = 0; i < 16; ++i) {
        V[i] = 0;
        keypad[i] = false;
    }
    for (int i = 0; i < pixels_size; ++i) {
        display[i] = 0;
    }
}    


/**
 * Updates the delay and sound timers.
 *
 * This function decrements the delay_timer and sound_timer if they are greater than zero.
 * If the sound_timer is greater than zero, the beep sound is started.
 * Once the sound_timer reaches zero, the beep sound is stopped.
 */

void Chip8::update_timers(){
    if (delay_timer > 0){
        delay_timer--;
    }
    if (sound_timer > 0){
        sound_timer--;
        player.start_beep();
        
    }else{
        player.stop_beep();
    }
}

/**
 * Loads a Chip8 ROM into the emulator's memory.
 *
 * This function attempts to open the given file and read its contents into the
 * emulator's memory. If the file cannot be opened or is too large to fit in the
 * emulator's memory, an error message is printed to the standard error stream,
 * and the function returns false. Otherwise, the function returns true.
 */
bool Chip8::loadROM(const char* path){
    std::ifstream rom_file(path, std::ios::binary);
    if(!rom_file.is_open()){
        std::cerr << "Failed to open ROM file." << std::endl;
        return false;
    }

    //file size
    rom_file.seekg(0, std::ios::end);
    std::size_t rom_size = rom_file.tellg();
    rom_file.seekg(0, std::ios::beg);

    if (rom_size > (mem_size - start_addr)){
        std::cerr << "ROM file is too large." << std::endl;
        return false;
    }

    rom_file.read(reinterpret_cast<char*>(memory + start_addr), rom_size);
    rom_file.close();
    return true;
}

/**
 * Emulates one cycle of the Chip-8 processor.
 *
 * This function fetches the next instruction from memory, decodes it, and
 * executes it. If the instruction is not recognized, an error message is
 * printed to the standard error stream.
 */
void Chip8::emulateCycle(){
    uint16_t instruction = fetchInstruction();
    //std::cout << std::hex << (int) instruction << std::endl;

    switch (opcode(instruction))
    {
        case 0:
            switch (instruction)
            {
                //00E0 - CLS
                case 0x00E0: 
                    x00E0_clr();
                    break;
                //00EE - RET
                case 0x00EE:
                    x00EE_return_from_subroutine();
                    break;
                default: break;
            }   
            break; 
        
        case 1://1nnn - JP addr
            x1nnn_jmp(nnn(instruction));
            break;          
        
        case 2: //2nnn - CALL addr
            x2nnn_call_subroutine(nnn(instruction));
            break;
        
        case 3 ://3xkk - SE Vx, byte
            x3xkk_skip_if_equal(X(instruction) , kk(instruction));
            break;
        
        case 4://4xkk - SNE Vx, byte
            x4xkk_skip_if_not_equal(X(instruction) , kk(instruction));
            break;
        
        case 5:  //5xy0 - SE Vx, Vy
            x5xy0_skip_if_equal_reg(X(instruction) , Y(instruction));
            break;

        case 6: //6xkk - LD Vx, byte
            x6xkk_set_reg(X(instruction) , kk(instruction));
            break;
    
        case 7: // 7xkk - ADD Vx, byte
            x7xkk_add_byte(X(instruction) , kk(instruction));
            break;

        case 8:
            switch (n(instruction))
            {
                case 0: //8xy0 - LD Vx, Vy
                    x8xy0_ld_reg(X(instruction) , Y(instruction));
                    break;
                
                case 1://8xy1 - OR Vx, Vy
                    x8xy1_OR(X(instruction) , Y(instruction));
                    break;
                
                case 2: //8xy2 - AND Vx, Vy
                    x8xy2_AND(X(instruction) , Y(instruction));
                    break;
                
                case 3: //8xy3 - XOR Vx, Vy
                    x8xy3_XOR(X(instruction) , Y(instruction));
                    break;
                
                case 4: //8xy4 - ADD Vx, Vy
                    x8xy4_add_reg_with_carry(X(instruction) , Y(instruction));
                    break;
                
                case 5: //8xy5 - SUB Vx, Vy
                    x8xy5_sub_reg(X(instruction) , Y(instruction));
                    break;
                
                case 6: //8xy6 - SHR Vx {, Vy}
                    x8xy6_shift_right(X(instruction) , Y(instruction));
                    break;
                
                case 7: //8xy7 - SUBN Vx, Vy
                    x8xy7_subn_reg(X(instruction) , Y(instruction));
                    break;
                
                case 0xE: //8xyE - SHL Vx {, Vy}
                    x8xyE_shift_left(X(instruction) , Y(instruction));
                    break;
                
                default:
                    std::cerr << "Unknown opcode: " << std::hex << instruction << std::endl;
                    break;
            }
            break;
        
        case 9://9xy0 - SNE Vx, Vy
            x9xy0_skip_if_not_equal_reg(X(instruction) , Y(instruction));
            break;
        
        case 0xA://Annn - LD I, addr
            xAnnn_setI(nnn(instruction));
            break;
        
        case 0xB: //Bnnn - JP V0, addr
            xBnnn_JPV0(nnn(instruction));
            break;
        
        case 0xC: //Cxkk - RND Vx, byte
            xCxkk_rnd(X(instruction) , kk(instruction));
            break;
        
        case 0xD:
            xDxyn_draw(X(instruction) , Y(instruction) , n(instruction));
            break;
        
        case 0xE:
            switch(kk(instruction)){
                case 0x9E: //Ex9E - SKP Vx
                    xEx9E_skip_if_key_pressed(X(instruction));
                    break;
                
                case 0xA1: //ExA1 - SKNP Vx
                    xExA1_skip_if_key_not_pressed(X(instruction));
                    break;
                
                default:
                    std::cerr << "Unknown opcode: " << std::hex << instruction << std::endl;
                    break;
            }
            break;
        
        case 0xF:
            switch(kk(instruction)){
                case 0x07: //Fx07 - LD Vx, DT
                    xFx07_set_reg_delay_timer(X(instruction));
                    break;
                
                case 0x0A: //Fx0A - LD Vx, K
                    xFx0A_wait_for_key_press(X(instruction));
                    break;
                
                case 0x15: //Fx15 - LD DT, Vx
                    xFx15_set_delay_timer_reg(X(instruction));
                    break;
                
                case 0x18://Fx18 - LD ST, Vx
                    xFx18_set_sound_timer_reg(X(instruction));
                    break;
                
                case 0x1E: //Fx1E - ADD I, Vx
                    xFx1E_add_to_I(X(instruction));
                    break;
                
                case 0x29://Fx29 - LD F, Vx
                    xFx29_set_I_to_font(X(instruction));
                    break;
                
                case 0x33: //Fx33 - LD B, Vx
                    xFx33_set_BCD(X(instruction));
                    break;
                
                case 0x55: //Fx55 - LD [I], Vx
                    xFx55_store_regs(X(instruction));
                    break;
                
                case 0x65: //Fx65 - LD Vx, [I]
                    xFx65_load_regs(X(instruction));
                    break;
                
                default:
                    std::cerr << "Unknown opcode: " << std::hex << instruction << std::endl;
                    break;
        }
        break;
        default:
            std::cerr << "Unknown opcode: " << std::hex << instruction << std::endl;
            break;
    }
}

    /**
     * @brief Fetches a Chip-8 instruction from memory and increments the program counter
     * @return The fetched instruction
     */
inline uint16_t Chip8::fetchInstruction(){
    uint16_t inst = 0 ;
    inst = (memory[pc] << 8) | memory[pc+1];
    pc += 2;
    return inst;
}

inline void Chip8::x00E0_clr(){
    memset(display, 0, sizeof(display));
    draw_flag = true; 
     
}

//1nnn - JP addr
inline void Chip8::x1nnn_jmp(uint16_t addr){
    pc = addr;
}

//6xkk - LD Vx, byte
inline void Chip8::x6xkk_set_reg(uint8_t reg , uint8_t value){
    V[reg] = value;
}

//7xkk - ADD Vx, byte
inline void Chip8::x7xkk_add_byte(uint8_t reg , uint8_t value){
    V[reg] += value;
}

inline void Chip8::x00EE_return_from_subroutine(){
    sp -= 1;
    pc = stack[sp];
}

//2nnn - CALL addr
inline void Chip8::x2nnn_call_subroutine(uint16_t addr){
    stack[sp] = pc;
    sp += 1;
    pc = addr;
}

inline void Chip8::x3xkk_skip_if_equal(uint8_t reg , uint8_t value){
    if(V[reg] == value)
        pc += 2;
}

//4xkk - SNE Vx, byte
inline void Chip8::x4xkk_skip_if_not_equal(uint8_t reg , uint8_t value){
    if(V[reg] != value)
        pc += 2;
}


inline void Chip8::x5xy0_skip_if_equal_reg(uint8_t reg_x , uint8_t reg_y){
    if(V[reg_x] == V[reg_y])
        pc += 2;
}

//8xy0 - LD Vx, Vy
inline void Chip8::x8xy0_ld_reg(uint8_t reg_x , uint8_t reg_y){
    V[reg_x] = V[reg_y];
}

//8xy1 - OR Vx, Vy
inline void Chip8::x8xy1_OR(uint8_t reg_x , uint8_t reg_y){
    V[reg_x] |= V[reg_y];
    V[0x0f] = 0;
}

//8xy2 - AND Vx, Vy
inline void Chip8::x8xy2_AND(uint8_t reg_x , uint8_t reg_y){
    V[reg_x] &= V[reg_y];
    V[0x0f] = 0;
}

//8xy3 - XOR Vx, Vy
inline void Chip8::x8xy3_XOR(uint8_t reg_x , uint8_t reg_y){
    V[reg_x] ^= V[reg_y];
    V[0x0f] = 0;
}

//8xy4 - ADD Vx, Vy
inline void Chip8::x8xy4_add_reg_with_carry(uint8_t reg_x , uint8_t reg_y){
    uint16_t result = V[reg_x] + V[reg_y];
    V[reg_x] = result ;
    V[0x0f] = (result > 0xFF) ? 1 : 0;
    
}

//8xy5 - SUB Vx, Vy
inline void Chip8::x8xy5_sub_reg(uint8_t reg_x , uint8_t reg_y){
    uint8_t carry = (V[reg_x] >= V[reg_y]) ? 1 : 0;
    V[reg_x] -= V[reg_y];
    V[0x0f] = carry;
}

//8xy6 - SHR Vx {, Vy}
//WIKI : shifted the value in the register VY and stored the result in VX
inline void Chip8::x8xy6_shift_right(uint8_t reg_x, uint8_t reg_y){    
    V[reg_x] = V[reg_y];
    uint8_t carry = V[reg_x] & 0x1;
    V[reg_x] >>= 1;
    V[0x0f] = carry;
}

//8xy7 - SUBN Vx, Vy
inline void Chip8::x8xy7_subn_reg(uint8_t reg_x , uint8_t reg_y){
    uint8_t carry = (V[reg_y] >= V[reg_x]) ? 1 : 0;
    V[reg_x] = V[reg_y] - V[reg_x];
    V[0x0f] = carry;
}

//8xyE - SHL Vx {, Vy}
inline void Chip8::x8xyE_shift_left(uint8_t reg_x,uint8_t reg_y){    
    V[reg_x] = V[reg_y];
    uint8_t carry = V[reg_x] >> 7;
    V[reg_x] <<= 1;
    V[0x0f] = carry;
}

//9xy0 - SNE Vx, Vy
inline void Chip8::x9xy0_skip_if_not_equal_reg(uint8_t reg_x , uint8_t reg_y){
    if(V[reg_x] != V[reg_y])
        pc += 2;
}

//Annn - LD I, addr
inline void Chip8::xAnnn_setI(uint16_t addr){
    I = addr;
}

//Bnnn - JP V0, addr
inline void Chip8::xBnnn_JPV0(uint16_t addr){
    pc = addr + V[0];
}

//Cxkk - RND Vx, byte
inline void Chip8::xCxkk_rnd(uint8_t reg , uint8_t value){
    V[reg] = (std::rand() % 256) & value;
}

inline void Chip8::xDxyn_draw(uint8_t xi, uint8_t yi , uint8_t n){
    uint8_t x = V[xi] % 64;
    uint8_t y = V[yi] % 32;
    V[0x0f] = 0;
    for(uint8_t row = 0 ; row < n ; ++row){
        uint8_t display_y = (y + row);
        if(display_y >= 32) break; // clip vertically
        uint8_t spirit_byte = memory[I + row];
        for(uint8_t col = 0 ; col < 8 ; ++col){
            uint8_t display_x = (x + col);
            if(display_x >= 64) break; // clip horizontally
            uint8_t pixel = readbit(spirit_byte, (7 - col));
            if(pixel){
                if(display[display_x + (display_y * 64)] == 1){
                    V[0x0f] = 1;
                }
                display[display_x + (display_y * 64)] ^= 1;    
            } 
        }
    }
    draw_flag = true;
}

//Ex9E - SKP Vx
inline void Chip8::xEx9E_skip_if_key_pressed(uint8_t reg){
    if(keypad[V[reg]]){
        pc += 2;
    }
}

//ExA1 - SKNP Vx
inline void Chip8::xExA1_skip_if_key_not_pressed(uint8_t reg){
    if(!keypad[V[reg]]){
        pc += 2;
    }
}

// Fx07 - LD Vx, DT
inline void Chip8::xFx07_set_reg_delay_timer(uint8_t reg){
    V[reg] = delay_timer;
}

// Fx0A - LD Vx, Key
inline void Chip8::xFx0A_wait_for_key_press(uint8_t reg){
    static bool key_pressed = false;
    
    //scan only if no key pressed
    if(key_pressed == false)
        for(uint8_t i = 0 ; i < 16 ; ++i){
            if(keypad[i]){
                V[reg] = i;
                key_pressed = true;
                break;
            }
        }
    //wait for key release
    else if(keypad[V[reg]] == 0){
        key_pressed = false;
        return;
    }
    pc-=2;
}

//Fx15 - LD DT, Vx
inline void Chip8::xFx15_set_delay_timer_reg(uint8_t reg){
    delay_timer = V[reg];
}

//Fx18 - LD ST, Vx
inline void Chip8::xFx18_set_sound_timer_reg(uint8_t reg){
    sound_timer = V[reg];
}

//Fx1E - ADD I, Vx
inline void Chip8::xFx1E_add_to_I(uint8_t reg){
    I += V[reg];
}

//Fx29 - LD F, Vx
inline void Chip8::xFx29_set_I_to_font(uint8_t reg){
    I =  (V[reg] * 5);
}

//Fx33 - LD B, Vx
inline void Chip8::xFx33_set_BCD(uint8_t reg){
    memory[I] = V[reg] / 100;
    memory[I + 1] = (V[reg] / 10) % 10;
    memory[I + 2] = V[reg] % 10;
}

//Fx55 - LD [I], Vx
inline void Chip8::xFx55_store_regs(uint8_t reg){
    for(uint8_t i = 0 ; i <= reg ; ++i){
        memory[I + i] = V[i];
    }
    I += reg + 1;
}

//Fx65 - LD Vx, [I]
inline void Chip8::xFx65_load_regs(uint8_t reg){
    for(uint8_t i = 0 ; i <= reg ; ++i){
        V[i] = memory[I + i];
    }
    I += reg + 1;
}


//helpers
inline uint8_t Chip8::readbit(uint8_t byte,uint8_t pos) {
    return (byte >> pos) & 0x01;
}
inline uint8_t Chip8::X(uint16_t instruction){
    return (instruction & 0x0F00) >> 8;
}

inline uint8_t Chip8::Y(uint16_t instruction){
    return (instruction & 0x00F0) >> 4;
}

inline uint16_t Chip8::nnn(uint16_t instruction){
    return instruction & 0x0FFF;
}

inline uint8_t Chip8::kk(uint16_t instruction){
    return instruction & 0x00FF;
}

inline uint8_t Chip8::n(uint16_t instruction){
    return instruction & 0x000F;
}

inline uint8_t Chip8::opcode(uint16_t instruction){
    return (instruction & 0xF000 )>> 12;
}