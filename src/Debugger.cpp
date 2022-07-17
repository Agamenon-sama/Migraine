#include "Debugger.h"

#include <glad/glad.h>

#include <iostream>

Debugger::Debugger(uint8_t *data) : Window("Debugger", 600, 700, true) {
    // createGLContext();
    _gui.create(this);

    _data = data;
    // char *mess = "Chams, you're the worst programmer that has ever lived";
    // memcpy(_data, mess, 55);
}

Debugger::~Debugger() {
    _gui.shutdown();
}

void Debugger::render() {
    SDL_GL_MakeCurrent(_window, _glContext);
    _gui.beginRender();
    _memEditor.DrawContents(_data, 1024);

    _gui.endRender();
}
