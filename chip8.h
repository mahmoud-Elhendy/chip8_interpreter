#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <ctime>
#include <string.h>

#include "beepplayer.h"

constexpr uint16_t pixels_size = 2048;
constexpr uint16_t mem_size = 4096;
constexpr uint16_t start_addr = 0x200;
constexpr uint8_t stack_size = 16; 
constexpr uint8_t chip8_fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};
constexpr uint8_t font_start_addr = 0x50;

//#define readbit(byte, pos) byte | 0x01 << pos

class Chip8 {
    
public:
    Chip8();
    void emulateCycle();
    bool loadROM(const char* path);
    void reset();
    void update_timers();
private:
    uint16_t fetchInstruction();    
    
    //helpers
    uint8_t readbit(uint8_t byte,uint8_t pos);
    uint8_t X(uint16_t instruction);
    uint8_t Y(uint16_t instruction);
    uint16_t nnn(uint16_t instruction);
    uint8_t kk(uint16_t instruction);
    uint8_t n(uint16_t instruction);
    uint8_t opcode(uint16_t instruction);

    //instructions
    //00E0 - CLS
    void x00E0_clr();
    //1nnn - JP addr
    void x1nnn_jmp(uint16_t addr);
    //6xkk - LD Vx, byte
    void x6xkk_set_reg(uint8_t reg , uint8_t value);
    //7xkk - ADD Vx, byte
    void x7xkk_add_byte(uint8_t reg , uint8_t value);
    //00EE - RET
    void x00EE_return_from_subroutine();
    //2nnn - CALL addr
    void x2nnn_call_subroutine(uint16_t addr);
    //3xkk - SE Vx, byte
    void x3xkk_skip_if_equal(uint8_t reg , uint8_t value);
    //4xkk - SNE Vx, byte
    void x4xkk_skip_if_not_equal(uint8_t reg , uint8_t value);
    //5xy0 - SE Vx, Vy
    void x5xy0_skip_if_equal_reg(uint8_t reg_x , uint8_t reg_y);
    
    //8xy0 - LD Vx, Vy
    void x8xy0_ld_reg(uint8_t reg_x , uint8_t reg_y);
    //8xy1 - OR Vx, Vy
    void x8xy1_OR(uint8_t reg_x , uint8_t reg_y);
    //8xy2 - AND Vx, Vy
    void x8xy2_AND(uint8_t reg_x , uint8_t reg_y);
    //8xy3 - XOR Vx, Vy    
    void x8xy3_XOR(uint8_t reg_x , uint8_t reg_y);
    //8xy4 - ADD Vx, Vy    
    void x8xy4_add_reg_with_carry(uint8_t reg_x , uint8_t reg_y);
    //8xy5 - SUB Vx, Vy    
    void x8xy5_sub_reg(uint8_t reg_x , uint8_t reg_y);
    //8xy6 - SHR Vx {, Vy}
    void x8xy6_shift_right(uint8_t reg_x, uint8_t reg_y);
    //8xy7 - SUBN Vx, Vy
    void x8xy7_subn_reg(uint8_t reg_x , uint8_t reg_y);
    //8xyE - SHL Vx {, Vy}
    void x8xyE_shift_left(uint8_t reg_x, uint8_t reg_y);
    //9xy0 - SNE Vx, Vy
    void x9xy0_skip_if_not_equal_reg(uint8_t reg_x , uint8_t reg_y);
    //Annn - LD I, addr
    void xAnnn_setI(uint16_t addr);
    //Bnnn - JP V0, addr
    void  xBnnn_JPV0(uint16_t addr);
    // Cxkk - RND Vx, byte
    void xCxkk_rnd(uint8_t reg , uint8_t value);
    //Dxyn - DRW Vx, Vy, nibble
    void xDxyn_draw(uint8_t x, uint8_t y , uint8_t n);  
   
    //Ex9E - SKP Vx
    void xEx9E_skip_if_key_pressed(uint8_t reg);
    //ExA1 - SKNP Vx
    void xExA1_skip_if_key_not_pressed(uint8_t reg);
    //Fx07 - LD Vx, DT
    void xFx07_set_reg_delay_timer(uint8_t reg);
    //Fx0A - LD Vx, K
    void xFx0A_wait_for_key_press(uint8_t reg);
    //Fx15 - LD DT, Vx
    void xFx15_set_delay_timer_reg(uint8_t reg);
    //Fx18 - LD ST, Vx
    void xFx18_set_sound_timer_reg(uint8_t reg);
    //Fx1E - ADD I, Vx
    void xFx1E_add_to_I(uint8_t reg);
    //Fx29 - LD F, Vx
    void xFx29_set_I_to_font(uint8_t reg);
    //Fx33 - LD B, Vx
    void xFx33_set_BCD(uint8_t reg);
    //Fx55 - LD [I], Vx
    void xFx55_store_regs(uint8_t reg);
    //Fx65 - LD Vx, [I]
    void xFx65_load_regs(uint8_t reg);

public:
    bool keypad[16];
    bool draw_flag;
    uint8_t display[pixels_size];
private:
    uint8_t memory[mem_size];
    uint16_t stack[stack_size];
    uint8_t V[16];
    uint16_t I;
    uint16_t pc;
    uint8_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;
    beepPlayer player;
};