#pragma once

#include <SDL2/SDL_keyboard.h>

#include "MessageBox.h"

enum class Chip8Key : int {
    K0 = SDL_SCANCODE_X,
    K1 = SDL_SCANCODE_1,
    K2 = SDL_SCANCODE_2,
    K3 = SDL_SCANCODE_3,
    K4 = SDL_SCANCODE_Q,
    K5 = SDL_SCANCODE_W,
    K6 = SDL_SCANCODE_E,
    K7 = SDL_SCANCODE_A,
    K8 = SDL_SCANCODE_S,
    K9 = SDL_SCANCODE_D,
    KA = SDL_SCANCODE_Z,
    KB = SDL_SCANCODE_C,
    KC = SDL_SCANCODE_4,
    KD = SDL_SCANCODE_R,
    KE = SDL_SCANCODE_F,
    KF = SDL_SCANCODE_V
};

class Keyboard {
public:
    static bool keyPressed(int key) {
        auto keys = SDL_GetKeyboardState(nullptr);
        return keys[toScanCode(key)];
    }

    static bool anyKeyPressed(uint8_t *vx) {
        // this line should be useless but just to calm the voices in my head
        if (!vx) { return false; }

        auto keys = SDL_GetKeyboardState(nullptr);

        for (int i = 0; i <= 0xf; i++) {
            if (keys[toScanCode(i)]) {
                *vx = i;
                return true;
            }
        }

        return false;
    }

private:
    static SDL_Scancode toScanCode(int key) {
        SDL_Scancode scanCodes[] = {
            SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
            SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
            SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
            SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
        };

        if (key < 0 || key > 0xf) {
            MessageBox::error(
                "The emulator is checking the keyboard for a key that doesn't exist. "
                "This is likely an issue from the rom file",
                "Invalid key press"
            );
            exit(4);
        }

        return scanCodes[key];
    }
};
