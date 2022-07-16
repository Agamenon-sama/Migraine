#include "Debugger.h"

#include <glad/glad.h>

#include <iostream>

Debugger::Debugger(uint8_t *data) : Window("Debugger", 600, 700, true) {
    createGLContext();
    // Load OpenGL. NEVER FORGET TO LOAD OpenGL
    if (!gladLoadGLLoader(((GLADloadproc) SDL_GL_GetProcAddress))) {
        std::cerr << "Failed to load OpenGL\n";
        // return 1;
    }
    _gui.create(this);

    _data = data;
    // char *mess = "Chams, you're the worst programmer that has ever lived";
    // memcpy(_data, mess, 55);
    // srand(time(0));
    // for(uint8_t x = 0; x < 1024; x++) {
    //     _data[x] = rand() % 255;
    // }
}

Debugger::~Debugger() {
    _gui.shutdown();
    // delete[] _data;
}

void Debugger::render() {
    _gui.beginRender();
    _memEditor.DrawContents(_data, 1024);

    _gui.endRender();
    swapBuffers();
}
