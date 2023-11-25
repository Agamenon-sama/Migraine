#include "chip8.h"
#include "system/MessageBox.h"
#include "system/Keyboard.h"

#include <fstream>
#include <sstream>

#include <string.h>

Chip8::Chip8() {
    // allocating memory
    stack = new uint16_t [16];
    v = new uint8_t [16];
    mem = new uint8_t [4096];

    // set registers and memory
    unset();

    // Seeding the rng
    rng.seed(std::chrono::steady_clock().now().time_since_epoch().count()); // I love chrono Black Magic :)

    // init framebuffer
    _clearFrameBuffer();
}

Chip8::~Chip8() {
    delete [] stack;
    delete [] v;
    delete [] mem;
}

void Chip8::_clearFrameBuffer() {
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            _frameBuffer[y][x] = 0;
        }
    }
}

void Chip8::reset() {
    // Initilizing register
    i = 0;
    sp = 0;
    pc = 0x0200;
    opcode = 0;
    delayTimer = 0;
    soundTimer = 0;

    memset(stack, 0, 2 * 16);
    memset(v, 0, 16);

    _clearFrameBuffer();
}

void Chip8::unset() {
    reset();

    // clear memory
    memset(mem, 0, 4096);

    _clearFrameBuffer();
}

bool Chip8::load(const std::string &path) {
    // Loading program
    std::ifstream romLoader(path, std::ios::in | std::ios::binary);
    if(!romLoader.is_open()) {
        MessageBox::warning("Failed to open file at " + path);
        return false;
    }

    romLoader.seekg(0, std::ios::end);
    auto size = romLoader.tellg();
    romLoader.seekg(0);
    romLoader.read((char *)mem + 0x200, size);

    romLoader.close();

    
    // Loading font set;
    uint8_t fontset[80] = { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for(uint8_t pixelIndex = 0; pixelIndex < 80; pixelIndex++) {
        mem[pixelIndex] = fontset[pixelIndex];
    }

    memcpy(mem + 0x100, "Emulator made by Agamenon", 26);

    return true;
}

/**
 * @brief Handles invalid opecodes
 * 
 * @param opecode invalid opcode
*/
static void invalidOpcode(uint16_t opcode) {
    std::stringstream ss;
    ss << std::hex << "opcode 0x" << opcode << " is undefined";
    MessageBox::error(ss.str(), "Problem in the rom file");
    exit(3);
}

void Chip8::emulateCycle() {
    uint8_t x, y, n;
    uint8_t nn;
    uint16_t nnn;

    // fetch
    opcode = mem[pc] << 8 | mem[pc + 1];
    x   = (opcode >> 8) & 0x000F; // the lower 4 bits of the high byte
    y   = (opcode >> 4) & 0x000F; // the upper 4 bits of the low byte
    n   = opcode & 0x000F; // the lowest 4 bits
    nn  = opcode & 0x00FF; // the lowest 8 bits
    nnn = opcode & 0x0FFF; // the lowest 12 bits

    // decode & execute: case on the highest order byte
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (nn) {
                case 0x00E0: // clear the screen
                    // chip8_draw_flag = true;
                    // _renderer->clear();
                    _clearFrameBuffer();
                    pc += 2;
                    break;
                case 0x00EE: // ret
                    sp--;
                    pc = stack[sp];
                    break;
                default:
                    invalidOpcode(opcode);
            }
            break;
        case 0x1000: // 1nnn: jump to address nnn
            pc = nnn;
            break;
        case 0x2000: // 2nnn: call address nnn
            stack[sp] = pc + 2;
            sp++;
            pc = nnn;
            break;
        case 0x3000: // 3xnn: skip next instr if v[x] = nn
            pc += (v[x] == nn) ? 4 : 2;
            break;
        case 0x4000: // 4xnn: skip next instr if v[x] != nn
            pc += (v[x] != nn) ? 4 : 2;
            break;
        case 0x5000: // 5xy0: skip next instr if v[x] == v[y]
            pc += (v[x] == v[y]) ? 4 : 2;
            break;
        case 0x6000: // 6xnn: set v[x] = nn
            v[x] = nn;
            pc += 2;
            break;
        case 0x7000: // 7xnn: set v[x] = v[x] + nn
            v[x] += nn;
            pc += 2;
            break;
        case 0x8000: // 8xyn: Arithmetic stuff
            switch (n) {
                case 0x0:
                    v[x] = v[y];
                    break;
                case 0x1: // or
                    v[x] = v[x] | v[y];
                    break;
                case 0x2: // and
                    v[x] = v[x] & v[y];
                    break;
                case 0x3: // xor
                    v[x] = v[x] ^ v[y];
                    break;
                case 0x4:
                    v[0xF] = ((int) v[x] + (int) v[y]) > 255 ? 1 : 0;
                    v[x] = v[x] + v[y];
                    break;
                case 0x5: 
                    v[0xF] = (v[x] > v[y]) ? 1 : 0;
                    v[x] = v[x] - v[y];
                    break;
                case 0x6:
                    v[0xF] = v[x] & 0x1;
                    v[x] = (v[x] >> 1);
                    break;
                case 0x7:
                    v[0xF] = (v[y] > v[x]) ? 1 : 0;
                    v[x] = v[y] - v[x];
                    break;
                case 0xE:
                    v[0xF] = (v[x] >> 7) & 0x1;
                    v[x] = (v[x] << 1);
                    break;
                default:
                    invalidOpcode(opcode);
            }
            pc += 2;
            break;
        case 0x9000: // 9xy0: skip instruction if Vx != Vy
            switch (n) {
                case 0x0:
                    pc += (v[x] != v[y]) ? 4 : 2;
                    break;
                default:
                    invalidOpcode(opcode);
            }
            break;
        case 0xA000: // Annn: set I to address nnn
            i = nnn;
            pc += 2;
            break;
        case 0xB000: // Bnnn: jump to location nnn + v[0]
            pc = nnn + v[0];
            break;
        case 0xC000: // Cxnn: v[x] = random byte AND nn
            v[x] = nn & (rng() % 256);
            pc += 2;
            break;
        case 0xD000: // Dxyn: Display an n-byte sprite starting at memory
                     // location I at (Vx, Vy) on the screen, VF = collision
            v[0xf] = 0;
            for (int row = 0; row < n; row++) {
                uint8_t sprite = mem[i + row];

                for (int column = 0; column < 8; column++) {
                    uint8_t spritePixel = sprite & (0x80 >> column);
                    uint8_t *screenPixel = &_frameBuffer[v[y] + row][v[x] + column];

                    if (spritePixel) { // if we have to draw, we draw
                        if (*screenPixel) { // but before modifying the screen, we test for collision
                            v[0xf] = 1;
                        }
                        // draw
                        *screenPixel ^= 1;
                    }
                }
            }

            pc += 2;
            // chip8_draw_flag = true;
            break;
        case 0xE000: // key-pressed events
            switch (nn) {
                case 0x9E: // skip next instr if key[Vx] is pressed
                    pc += Keyboard::keyPressed(v[x]) ? 4 : 2;
                    break;
                case 0xA1: // skip next instr if key[Vx] is not pressed
                    pc += !Keyboard::keyPressed(v[x]) ? 4 : 2;
                    break;
                default:
                    invalidOpcode(opcode);
            }
            break;
        case 0xF000: // misc
            switch (nn) {
                case 0x07:
                    v[x] = delayTimer;
                    pc += 2;
                    break;
                case 0x0A:
                    // if any key is pressed, we move to the next instruction
                    // else, we keep executing the same instruction
                    // thus implementing some sort of block without halting the process
                    pc += Keyboard::anyKeyPressed(&v[x]) ? 2 : 0;
                    break;
                case 0x15:
                    delayTimer = v[x];
                    pc += 2;
                    break;
                case 0x18:
                    soundTimer = v[x];
                    pc += 2;
                    break;
                case 0x1E:
                    v[0xf] = (i + v[x] > 0xfff) ? 1 : 0;
                    i += v[x];
                    pc += 2;
                    break;
                case 0x29:
                    i = v[x] * 5; // 5 is the size of each character sprite in memory
                    pc += 2;
                    break;
                case 0x33:
                    mem[i]   = (v[x] % 1000) / 100; // hundreds
                    mem[i+1] = (v[x] % 100) / 10;   // tens
                    mem[i+2] = (v[x] % 10);         // ones
                    pc += 2;
                    break;
                case 0x55:
                    for (int j = 0; j <= x; j++) {
                        mem[i + j] = v[j];
                    }
                    // apparently different chip8 implementations behave differently
                    // when it comes to whether you increment i or not
                    // I guess that by default, I won't increment as that's the most modern way
                    // todo: implement a setting to configure this behaviour by the user
                    // i += x + 1;
                    pc += 2;
                    break;
                case 0x65:
                    for (int j = 0; j <= x; j++) {
                        v[j] = mem[i + j];
                    }
                    // i += x + 1;
                    pc += 2;
                    break;
                default:
                    invalidOpcode(opcode);
            }
            break;
        default:
            invalidOpcode(opcode);
    }
}
