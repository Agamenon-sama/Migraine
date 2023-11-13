#pragma once

#include <SDL2/SDL.h>

#include "chip8.h"

class Renderer {
public:
    Renderer(SDL_Window *window, Chip8 *chip8);
    ~Renderer();

    void clear();
    void setFrameMap(const uint8_t x, const uint8_t y, const uint8_t n);
    void render();
    void swapBuff(SDL_Window *win);

private:
    // uint8_t _frameMap[32][64];
    uint8_t *_image;
    uint32_t _vao;
    uint32_t _vbo;
    uint32_t _texture;
    uint32_t _shader;

    Chip8 *_chip8;
};

