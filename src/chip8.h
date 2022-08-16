#pragma once

#include <iostream>
#include <cstdint>
#include <string>
#include <random>
#include <chrono>

#include "Renderer.h"

class Chip8 {
    public:
    Chip8(Renderer *renderer);
    ~Chip8();

    bool load(const std::string &path);
    void reset();
    void unset();
    void emulateCycle();


    uint8_t *v; // These are the main cpu registers
    uint16_t i; // Address register
    uint16_t *stack; // The stack stores the pc before jumping through a branch
    uint16_t sp; // Stack pointer
    uint16_t pc; // Program counter
    uint16_t opcode;
    uint8_t delayTimer;
    uint8_t soundTimer;

    uint8_t *mem; // RAM

    std::mt19937 rng;

    private:
    Renderer *_renderer;
};
