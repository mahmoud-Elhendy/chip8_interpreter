# CHIP-8 Interpreter

## Overview

This project is a CHIP-8 interpreter implemented in C++. It emulates the behavior of the CHIP-8 virtual machine.[tested only on Ubuntu 20.04]

## Requirements

- SDL2 installed
  ```bash
   sudo apt-get install libsdl2-dev
   ```

## Building

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/chip8-interpreter.git
   cd chip8-interpreter
   ```
2. Create a build directory:
   ```bash
   mkdir build && cd build
   ```
3. Build the project:
   ```bash
   cmake .. && make
   ```

## Usage

Run the interpreter with a CHIP-8 ROM file:

```bash
./chip8 path/to/rom.ch8
```

### Controls

- CHIP-8 Keypad: Mapped to the following keys:
  ```
  1 2 3 4       =>       1 2 3 C
  Q W E R       =>       4 5 6 D
  A S D F       =>       7 8 9 E
  Z X C V       =>       A 0 B F
  ```
- Exit: Press `ESC` or close the terminal.

## Rendering

The interpreter renders the CHIP-8 display (64x32 resolution) uisng SDL

## Sound

A beep sound is generated when the sound timer is active. The implementation uses a portable method to play sound across platforms.


## ROMS

The ROMs added from : https://github.com/kripod/chip8-roms
