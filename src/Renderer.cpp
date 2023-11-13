#include "Renderer.h"

#include <glad/glad.h>

#include <random>
#include <chrono>
#include <iostream>

std::mt19937 rng(std::chrono::steady_clock().now().time_since_epoch().count());

Renderer::Renderer(SDL_Window *window, Chip8 *chip8) {
    _chip8 = chip8;

    // opengl buffers
    float vertices[6*4] = {
        // position   // texture coord
         1.0f,  1.0f, 1.0f, 1.0f, // top right
        -1.0f,  1.0f, 0.0f, 1.0f, // top left
        -1.0f, -1.0f, 0.0f, 0.0f, // bot left

         1.0f,  1.0f, 1.0f, 1.0f, // top right
        -1.0f, -1.0f, 0.0f, 0.0f, // bot left
         1.0f, -1.0f, 1.0f, 0.0f  // bot right
    };

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)0);
    glEnableVertexAttribArray(1); // texCoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)(2 * sizeof(float)));

    // opengl texture
    _image = new uint8_t[64 * 32 * 3];
    // todo: the following is just for test. remove it later
    memset(_image, 0, 64*32*3);
    _image[0*3] = 255; _image[0*3 + 1] = 255; _image[0*3 + 2] = 255;
    _image[100*3] = 255; _image[100*3 + 1] = 255; _image[100*3 + 2] = 255;
    _image[200*3] = 255; _image[200*3 + 1] = 255; _image[200*3 + 2] = 255;

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, _image);

    // shader
    const char *vertexSource = R"(
        #version 330 core

        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;

        out vec2 texCoord;

        void main() {
            texCoord = aTexCoord;
            gl_Position = vec4(aPos, 1.0f, 1.0f);
        }
    )";
    const char *fragmentSource = R"(
        #version 330 core

        out vec4 fragColor;

        in vec2 texCoord;

        uniform sampler2D uTex;

        void main() {
            fragColor = texture(uTex, texCoord);
        }
    )";

    uint32_t vertexShader, fragmentShader;
    int success;
    char buildLog[512];
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, buildLog);
        std::cerr << "Vertex shader build error:\n" << buildLog << "\n";
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, buildLog);
        std::cerr << "Fragment shader build error:\n" << buildLog << "\n";
    }

    _shader = glCreateProgram();
    glAttachShader(_shader, vertexShader);
    glAttachShader(_shader, fragmentShader);
    glLinkProgram(_shader);
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(_shader, 512, nullptr, buildLog);
        std::cerr << "Shader link error:\n" << buildLog << "\n";
    }

    glDetachShader(_shader, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(_shader, fragmentShader);
    glDeleteShader(fragmentShader);
}

Renderer::~Renderer() {
    delete _image;
    glDeleteProgram(_shader);
    glDeleteTextures(1, &_texture);
    glDeleteBuffers(1, &_vbo);
}

void Renderer::setFrameMap(const uint8_t x, const uint8_t y, const uint8_t n) {
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
    // draw();
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

void Renderer::render() {
    // Clearing the frame buffer
    clear();

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            uint8_t pixel = _chip8->_frameBuffer[y][x] ? 255 : 0;
            // uint8_t pixel = rng() % 255;
            _image[((31 - y)*64 + x) * 3] = pixel;
            _image[((31 - y)*64 + x) * 3 + 1] = pixel;
            _image[((31 - y)*64 + x) * 3 + 2] = pixel;
        }
    }

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    glUseProgram(_shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, _image);

    glDrawArrays(GL_TRIANGLES, 0, 6);

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
