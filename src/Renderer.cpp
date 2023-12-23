#include "Renderer.h"
#include "system/MessageBox.h"

#include <glad/glad.h>
#include <SDL2/SDL_timer.h>

#include <iostream>
#include <string.h>

Renderer::Renderer(Chip8 *chip8) : _chip8(chip8) {
    // setting the color
    _onColor[0] = 255;  _offColor[0] = 0;
    _onColor[1] = 255;  _offColor[1] = 0;
    _onColor[2] = 255;  _offColor[2] = 0;
    
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
        uniform vec2 uResolution;
        uniform float uTime;
        uniform bool uCrtEffect;

        float noise(vec2 uv) {
            // this is really not that random and I don't like it
            float x = fract(sin(dot(uv.xy, vec2(25.967, 78.102))) * uTime);
            return x * 2.f - 1.f;
        }

        void main() {
            vec2 uv = gl_FragCoord.xy / uResolution;
            vec3 col = vec3(texture(uTex, texCoord));

            if (uCrtEffect) {
                // chromatic aberration
                col.r = texture(uTex, vec2(texCoord.x - 0.003, texCoord.y)).r;
                col.b = texture(uTex, vec2(texCoord.x + 0.003, texCoord.y)).b;

                // scanlines
                col += sin(uv.y * 1.6f * uResolution.y + uTime * 11.f) * 0.1f;

                // noise
                col += noise(uv) * 0.015f;

                // flickering
                col += sin(uTime * 100.f) * 0.01;

                // vignette
                vec2 v = uv * (1.0 - uv.yx);
                float vig = pow(v.x * v.y * 20.f, 0.3f);
                col *= vig;
            }

            fragColor = vec4(col, 1.f);
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
        MessageBox::error("Error building shader", "Shader error");
    }

    glDetachShader(_shader, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(_shader, fragmentShader);
    glDeleteShader(fragmentShader);

    // set resolution in shader
    glUseProgram(_shader);
    
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glUniform2f(glGetUniformLocation(_shader, "uResolution"), (float)viewport[2], (float)viewport[3]);

    _crtEffect = true;
    glUniform1i(glGetUniformLocation(_shader, "uCrtEffect"), _crtEffect);
}

Renderer::~Renderer() {
    delete _image;
    glDeleteProgram(_shader);
    glDeleteTextures(1, &_texture);
    glDeleteBuffers(1, &_vbo);
}

void Renderer::crtEffect() {
    // This doesn't update the uniform which is instead updated in the rendering
    // function because I can't guarantee I'm using the right opengl context
    // without making the Renderer aware of the Window which is ugly
    _crtEffect = !_crtEffect;
}

void Renderer::setOnColor(uint8_t r, uint8_t g, uint8_t b) {
    _onColor[0] = r;
    _onColor[1] = g;
    _onColor[2] = b;
}
void Renderer::setOffColor(uint8_t r, uint8_t g, uint8_t b) {
    _offColor[0] = r;
    _offColor[1] = g;
    _offColor[2] = b;
}

void Renderer::clear() {
    glClearColor(0.11, 0.11, 0.11, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::render() {
    // Clearing the frame buffer
    clear();

    // rendering the image
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            _image[((31 - y)*64 + x) * 3]     = _chip8->_frameBuffer[y][x] ? _onColor[0] : _offColor[0];
            _image[((31 - y)*64 + x) * 3 + 1] = _chip8->_frameBuffer[y][x] ? _onColor[1] : _offColor[1];
            _image[((31 - y)*64 + x) * 3 + 2] = _chip8->_frameBuffer[y][x] ? _onColor[2] : _offColor[2];
        }
    }

    // uploading it to opengl for drawing
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    glUseProgram(_shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 32, GL_RGB, GL_UNSIGNED_BYTE, _image);

    // update uniforms
    float t = SDL_GetTicks64() / 1'000.f;
    glUniform1f(glGetUniformLocation(_shader, "uTime"), t);
    glUniform1i(glGetUniformLocation(_shader, "uCrtEffect"), _crtEffect);

    // drawing
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
