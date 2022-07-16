#include <fstream>

#include "chip8.h"

Chip8::Chip8() {
    // Initilizing register
    i = 0;
    sp = 0;
    pc = 0x0200;
    opcode = 0;
    delayTimer = 0;
    soundTimer = 0;

    // allocating memory
    stack = new uint16_t [16];
    v = new uint8_t [16];
    mem = new uint8_t [4096];

    // Seeding the rng
    rng.seed(std::chrono::steady_clock().now().time_since_epoch().count()); // I love chrono Black Magic :)
}

Chip8::~Chip8() {
    delete [] stack;
    delete [] v;
    delete [] mem;
}

bool Chip8::load(const std::string &path) {
    // Loading program
    std::ifstream romLoader(path, std::ios::in | std::ios::binary);
    if(!romLoader.is_open()) {
        std::cerr << "Error : Failed to open the file " << path << "\n";
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
    return true;
}

bool Chip8::dumpMem() {
    std::cout << std::hex;
    for(uint16_t j = 0x200; j <= 4096; j++) {
        std::cout << mem[j]; // << "\t";
    }
    std::cout << std::endl;
    return true;
}

static void unvalidOpcode(uint16_t opcode) {
    std::cerr << std::hex << "opcode " << opcode << " is undefined\n";
    // chip->dumpMem(); todo: fix
    exit(1);
}

void Chip8::emulateCycle() {
    int i;
    uint8_t x, y, n;
    uint8_t kk;
    uint16_t nnn;

    // fetch
    opcode = mem[pc] << 8 | mem[pc + 1];
    x   = (opcode >> 8) & 0x000F; // the lower 4 bits of the high byte
    y   = (opcode >> 4) & 0x000F; // the upper 4 bits of the low byte
    n   = opcode & 0x000F; // the lowest 4 bits
    kk  = opcode & 0x00FF; // the lowest 8 bits
    nnn = opcode & 0x0FFF; // the lowest 12 bits

#ifdef DEBUG 
    printf("pc: 0x%04x Op: 0x%04x\n", pc, opcode);
#endif

    // decode & execute: case on the highest order byte
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (kk) {
                case 0x00E0: // clear the screen
                    // p("Clear the screen\n");
                    // memset(gfx, 0, sizeof(uint8_t) * GFX_SIZE);
                    // chip8_draw_flag = true;
                    // renderer->clear(); todo: fix
                    pc += 2;
                    break;
                case 0x00EE: // ret
                    // p("ret\n");
                    pc = stack[--sp];
                    break;
                default:
                    unvalidOpcode(opcode);
            }
            break;
        case 0x1000: // 1nnn: jump to address nnn
            // p("Jump to address 0x%x\n", nnn);
            pc = nnn;
            break;
        case 0x2000: // 2nnn: call address nnn
            // p("Call address 0x%x\n", nnn);
            stack[sp++] = pc + 2;
            pc = nnn;
            break;
        case 0x3000: // 3xkk: skip next instr if v[x] = kk
            // p("Skip next instruction if 0x%x == 0x%x\n", v[x], kk);
            pc += (v[x] == kk) ? 4 : 2;
            break;
        case 0x4000: // 4xkk: skip next instr if v[x] != kk
            // p("Skip next instruction if 0x%x != 0x%x\n", v[x], kk);
            pc += (v[x] != kk) ? 4 : 2;
            break;
        case 0x5000: // 5xy0: skip next instr if v[x] == v[y]
            // p("Skip next instruction if 0x%x == 0x%x\n", v[x], v[y]);
            pc += (v[x] == v[y]) ? 4 : 2;
            break;
        case 0x6000: // 6xkk: set v[x] = kk
            // p("Set v[0x%x] to 0x%x\n", x, kk);
            v[x] = kk;
            pc += 2;
            break;
        case 0x7000: // 7xkk: set v[x] = v[x] + kk
            // p("Set v[0x%d] to v[0x%d] + 0x%x\n", x, x, kk);
            v[x] += kk;
            pc += 2;
            break;
        case 0x8000: // 8xyn: Arithmetic stuff
            switch (n) {
                case 0x0:
                    // p("v[0x%x] = v[0x%x] = 0x%x\n", x, y, v[y]);
                    v[x] = v[y];
                    break;
                case 0x1:
                    // p("v[0x%x] |= v[0x%x] = 0x%x\n", x, y, v[y]);
                    v[x] = v[x] | v[y];
                    break;
                case 0x2:
                    // p("v[0x%x] &= v[0x%x] = 0x%x\n", x, y, v[y]);
                    v[x] = v[x] & v[y];
                    break;
                case 0x3:
                    // p("v[0x%x] ^= v[0x%x] = 0x%x\n", x, y, v[y]);
                    v[x] = v[x] ^ v[y];
                    break;
                case 0x4:
                    // p("v[0x%x] = v[0x%x] + v[0x%x] = 0x%x + 0x%x\n", x, x, y, v[x], v[y]);
                    v[0xF] = ((int) v[x] + (int) v[y]) > 255 ? 1 : 0;
                    v[x] = v[x] + v[y];
                    break;
                case 0x5: 
                    // p("v[0x%x] = v[0x%x] - v[0x%x] = 0x%x - 0x%x\n", x, x, y, v[x], v[y]);
                    v[0xF] = (v[x] > v[y]) ? 1 : 0;
                    v[x] = v[x] - v[y];
                    break;
                case 0x6:
                    // p("v[0x%x] = v[0x%x] >> 1 = 0x%x >> 1\n", x, x, v[x]);
                    v[0xF] = v[x] & 0x1;
                    v[x] = (v[x] >> 1);
                    break;
                case 0x7:
                    // p("v[0x%x] = v[0x%x] - v[0x%x] = 0x%x - 0x%x\n", x, y, x, v[y], v[x]);
                    v[0xF] = (v[y] > v[x]) ? 1 : 0;
                    v[x] = v[y] - v[x];
                    break;
                case 0xE:
                    // p("v[0x%x] = v[0x%x] << 1 = 0x%x << 1\n", x, x, v[x]);
                    v[0xF] = (v[x] >> 7) & 0x1;
                    v[x] = (v[x] << 1);
                    break;
                default:
                    unvalidOpcode(opcode);
            }
            pc += 2;
            break;
        case 0x9000: // 9xy0: skip instruction if Vx != Vy
            switch (n) {
                case 0x0:
                    // p("Skip next instruction if 0x%x != 0x%x\n", v[x], v[y]);
                    pc += (v[x] != v[y]) ? 4 : 2;
                    break;
                default:
                    unvalidOpcode(opcode);
            }
            break;
        case 0xA000: // Annn: set I to address nnn
            // p("Set I to 0x%x\n", nnn);
            i = nnn;
            pc += 2;
            break;
        case 0xB000: // Bnnn: jump to location nnn + v[0]
            // p("Jump to 0x%x + v[0] (0x%x)\n", nnn, v[0]);
            pc = nnn + v[0];
            break;
        case 0xC000: // Cxkk: v[x] = random byte AND kk
            // p("v[0x%x] = random byte\n", x);
            v[x] = kk & (rng() % 256);
            pc += 2;
            break;
        case 0xD000: // Dxyn: Display an n-byte sprite starting at memory
                     // location I at (Vx, Vy) on the screen, VF = collision
            // p("Draw sprite at (v[0x%x], v[0x%x]) = (0x%x, 0x%x) of height %d", 
            //  x, y, v[x], v[y], n);
            // draw_sprite(v[x], v[y], n);
            // renderer->render(v[x], v[y], n); todo: fix
            pc += 2;
            // chip8_draw_flag = true;
            break;
        case 0xE000: // key-pressed events
            switch (kk) {
                case 0x9E: // skip next instr if key[Vx] is pressed
                    // p("Skip next instruction if key[%d] is pressed\n", x);
                    // pc += (key[v[x]]) ? 4 : 2;
                    break;
                case 0xA1: // skip next instr if key[Vx] is not pressed
                    // p("Skip next instruction if key[%d] is NOT pressed\n", x);
                    // pc += (!key[v[x]]) ? 4 : 2;
                    break;
                default:
                    unvalidOpcode(opcode);
            }
            break;
        case 0xF000: // misc
            switch (kk) {
                case 0x07:
                    // p("v[0x%x] = delay timer = %d\n", x, delay_timer);
                    // v[x] = delay_timer;
                    pc += 2;
                    break;
                case 0x0A:
                    // i = 0;
                    // printf("Wait for key instruction\n");
                    // while (true) {
                    //     for (i = 0; i < KEY_SIZE; i++) {
                    //         if (key[i]) {
                    //             v[x] = i;
                    //             goto got_key_press;
                    //         }
                    //     }
                    // }
                    // got_key_press:
                    pc += 2;
                    break;
                case 0x15:
                    // p("delay timer = v[0x%x] = %d\n", x, v[x]);
                    // delay_timer = v[x];
                    pc += 2;
                    break;
                case 0x18:
                    // p("sound timer = v[0x%x] = %d\n", x, v[x]);
                    // sound_timer = v[x];
                    pc += 2;
                    break;
                case 0x1E:
                    // p("I = I + v[0x%x] = 0x%x + 0x%x\n", x, I, v[x]);
                    v[0xF] = (i + v[x] > 0xfff) ? 1 : 0;
                    i = i + v[x];
                    pc += 2;
                    break;
                case 0x29:
                    // p("I = location of font for character v[0x%x] = 0x%x\n", x, v[x]);
                    // I = FONTSET_BYTES_PER_CHAR * v[x];
                    pc += 2;
                    break;
                case 0x33:
                    // p("Store BCD for %d starting at address 0x%x\n", v[x], I);
                    mem[i]   = (v[x] % 1000) / 100; // hundred's digit
                    mem[i+1] = (v[x] % 100) / 10;   // ten's digit
                    mem[i+2] = (v[x] % 10);         // one's digit
                    pc += 2;
                    break;
                case 0x55:
                    // p("Copy sprite from registers 0 to 0x%x into memory at address 0x%x\n", x, I);
                    for (i = 0; i <= x; i++) { mem[i + i] = v[i]; }
                    i += x + 1;
                    pc += 2;
                    break;
                case 0x65:
                    // p("Copy sprite from memory at address 0x%x into registers 0 to 0x%x\n", x, I);
                    for (i = 0; i <= x; i++) { v[i] = mem[i + i]; }
                    i += x + 1;
                    pc += 2;
                    break;
                default:
                    unvalidOpcode(opcode);
            }
            break;
        default:
            unvalidOpcode(opcode);
}
}
