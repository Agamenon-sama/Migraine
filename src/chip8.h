#pragma once

#include <cstdint>
#include <string>
#include <random>

/**
 * @brief Implementation of the emulator
*/
class Chip8 {
public:
    Chip8();
    ~Chip8();

    /**
     * @brief Loads rom file and fonts
     * 
     * @param path path to the rom file
    */
    bool load(const std::string &path);
    /**
     * @brief Restart execution of already loaded rom file
     * 
     * This function resets the registers to their original values
    */
    void reset();
    /**
     * @brief Unloads the rom file and resets the registers
     * 
     * This function basically functions like reset() but also resets the memory
     * removing the rom and the fonts
    */
    void unset();
    /**
     * @brief Reads and executes 1 opcode
     * 
     * Emulates 1 fetch-decode-execute operation
    */
    void emulateCycle();


    uint8_t *v; ///< @brief These are the main cpu registers
    uint16_t i; ///< @brief Address register
    uint16_t *stack; ///< @brief The stack stores the pc before jumping through a branch
    uint16_t sp; ///< @brief Stack pointer
    uint16_t pc; ///< @brief Program counter
    uint16_t opcode; ///< @brief Currently loaded opcode
    uint8_t delayTimer; ///< @brief Timer used for timing and delays
    uint8_t soundTimer; ///< @brief Timer used to play sound when nonzero

    uint8_t *mem; ///< @brief RAM

    std::mt19937 rng; ///< @brief Random number generator used by 0xCXNN instruction

    uint8_t _frameBuffer[32][64]; ///< @brief Frame buffer holding current pixel values

    int breakPointAddress; ///< @brief Address at which we pause execution
    bool pause; ///< @brief Determines if we execute or not for debugging purposes

private:
    /**
     * @brief Sets all pixel values to off
    */
    void _clearFrameBuffer();
};
