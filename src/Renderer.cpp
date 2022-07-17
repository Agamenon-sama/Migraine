#include "Renderer.h"

#include <glad/glad.h>

#include <random>
#include <chrono>

std::mt19937 rng(std::chrono::steady_clock().now().time_since_epoch().count());

Renderer::Renderer(SDL_Window *window) {
    // _renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    // drawFlag = false;
    // for(int i = 0; i < 32; i++) {
    //     for(int j = 0; j < 64; j++) {
    //         _frameMap[i][j] = 0; //rng() & 1; // setting the map to random noise
    //     }
    // }
}

Renderer::~Renderer() {
    // SDL_DestroyRenderer(_renderer);
}

void Renderer::render(const uint8_t x, const uint8_t y, const uint8_t n) {
    // chip->v[0xF] = 0; // init to 0 todo: fix
    // todo: I really need to recheck this. I'm sure it's wrong
    // for(uint8_t row = 0; row < n; row++) {
        // uint8_t sprite = chip->mem[chip->i + row]; // Current row of pixels todo: fix

        // for(uint8_t col = 0; col < 8; col++){
            // uint8_t bit = (sprite >> col) & 0x1; // Current bit in the sprite todo: fix
            // uint8_t *pixel = &_frameMap[(y + col) % 32][(x + (7 - row) % 64)]; // Current screen pixel

            // if(bit && *pixel) { // If both are set to 1 a collision will happen
                // chip->v[0xF] = 1; // so we set vf to 1 todo: fix
            // }
            // *pixel = *pixel ^ bit;
        // }
    // }
    draw();
}

/*void Renderer::render(const uint8_t x, const uint8_t y, const uint8_t n) {
    unsigned row = y, col = x;
    unsigned byte_index;
    unsigned bit_index;

    // set the collision flag to 0
    chip->v[0xF] = 0;
    for (byte_index = 0; byte_index < n; byte_index++) {
        uint8_t byte = chip->mem[chip->i + byte_index];

        for (bit_index = 0; bit_index < 8; bit_index++) {
            // the value of the bit in the sprite
            uint8_t bit = (byte >> bit_index) & 0x1;
            // the value of the current pixel on the screen
            uint8_t *pixelp = &_frameMap[(row + byte_index) % 32]
                                        [(col + (7 - bit_index)) % 64];

            // if drawing to the screen would cause any pixel to be erased,
            // set the collision flag to 1
            if (bit == 1 && *pixelp ==1) chip->v[0xF] = 1;

            // draw this pixel by XOR
            *pixelp = *pixelp ^ bit;
        }
    }
    draw();
}*/

/*void Renderer::render(const uint8_t x, const uint8_t y, const uint8_t n) {
    for(int i = 0; i < 32; i++) {
        for(int j = 0; j < 64; j++) {
            _frameMap[i][j] = rng() & 1; // setting the map to random noise
        }
    }
    draw();
}*/

void Renderer::clear() {
    // SDL_SetRenderDrawColor(_renderer, 0xff, 0x50, 0xff, 0xff);
    // SDL_RenderClear(_renderer);
    glClearColor(0.11, 0.11, 0.11, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::draw() {
    // Clearing the frame buffer
    clear();

    // Drawing pixels
    // SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0xff);
    // for(int i = 0; i < 32; i++) {
    //     for(int j = 0; j < 64; j++) {
    //         if(_frameMap[i][j]) {
    //             SDL_Rect rect = {15*j, 15*i, 15, 15};
    //             SDL_RenderFillRect(_renderer, &rect);
    //         }
    //     }
    // }
}

void Renderer::swapBuff(SDL_Window *win) {
    // SDL_SetRenderDrawColor(_renderer, 0x1e, 0x05, 0x12, 0xff);
    // SDL_Rect rect = {100, 100, 20, 20};
    // SDL_RenderFillRect(_renderer, &rect);
    // SDL_RenderPresent(_renderer);
    SDL_GL_SwapWindow(win);
}
