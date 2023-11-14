#pragma once

#include "chip8.h"

/**
 * @brief Implements the OpenGL renderer for the emulator screen
*/
class Renderer {
public:
    /**
     * @brief Constructor for the renderer
     * 
     * @param chip8 pointer to the chip8 machine emulator used to access the machine's frame buffer
    */
    Renderer(Chip8 *chip8);
    ~Renderer();

    /**
     * @brief Clears screen frame buffer
    */
    void clear();
    /**
     * @brief Draws the chip8 frame buffer to the screen
    */
    void render();

private:
    uint8_t *_image; ///< @brief CPU side frame buffer
    uint32_t _vao;
    uint32_t _vbo;
    uint32_t _texture; ///< @brief GPU side frame buffer
    uint32_t _shader;

    Chip8 *_chip8; ///< @brief Non-owning pointer to chip8 machine
};

