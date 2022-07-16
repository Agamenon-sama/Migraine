#pragma once

#include <SDL2/SDL.h>

class Renderer {
public:
    Renderer(SDL_Window *window);
    ~Renderer();

    void clear();
    void render(const uint8_t x, const uint8_t y, const uint8_t n);
    void draw();
    void swapBuff();

    // bool drawFlag;
private:
    SDL_Renderer *_renderer;
    uint8_t _frameMap[32][64];
};

