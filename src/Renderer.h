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

    /**
     * @brief Sets the RGB color of lit pixels
    */
    void setOnColor(uint8_t r, uint8_t g, uint8_t b);
    /**
     * @brief Sets the RGB color of unlit pixels
    */
    void setOffColor(uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Toggles on and off the crt effect
    */
    void crtEffect();

private:
    uint8_t *_image; ///< @brief CPU side frame buffer
    uint32_t _vao; ///< @brief OpenGL vertex array
    uint32_t _vbo; ///< @brief OpenGL vertex buffer
    uint32_t _texture; ///< @brief GPU side frame buffer
    uint32_t _shader; ///< @brief OpenGL shader
    
    bool _crtEffect; ///< @brief stores whether we use the crt effect or not
    uint8_t _onColor[3];  ///< @brief RGB color of lit pixels
    uint8_t _offColor[3]; ///< @brief RGB color of unlit pixels

    Chip8 *_chip8; ///< @brief Non-owning pointer to chip8 machine
};

