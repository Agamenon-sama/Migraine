#pragma once

#include <SDL2/SDL_keyboard.h>

#include "MessageBox.h"

/**
 * @brief Provides Keyboard related operations for the chip8 emulator
*/
class Keyboard {
public:
    /**
     * @brief Tells if the button indicated by `key` is pressed
     * 
     * @param key a value from 0x0 to 0xf indicating the chip8 keyboard key
     * 
     * @returns true if the indicated key is pressed, false otherwise
    */
    static bool keyPressed(int key) {
        auto keys = SDL_GetKeyboardState(nullptr);
        return keys[_toScanCode(key)];
    }

    /**
     * @brief Tells if any button is pressed on the keyboard
     * 
     * @param vx pointer to where to write the pressed key which is a value between 0x0 and 0xf
     * 
     * @returns true if any button is pressed, false otherwise
    */
    static bool anyKeyPressed(uint8_t *vx) {
        // this line should be useless but just to calm the voices in my head
        if (!vx) { return false; }

        auto keys = SDL_GetKeyboardState(nullptr);

        for (int i = 0; i <= 0xf; i++) {
            if (keys[_toScanCode(i)]) {
                *vx = i;
                return true;
            }
        }

        return false;
    }

private:
    /**
     * @brief Converts the key value to an SDL_Scancode
     * 
     * @param key a value from 0x0 to 0xf indicating the chip8 keyboard key
     * 
     * @returns an SDL_Scancode value for the equivalent physical keyboard buttons
    */
    static SDL_Scancode _toScanCode(int key) {
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
