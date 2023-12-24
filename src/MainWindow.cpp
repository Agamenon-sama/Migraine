#include "MainWindow.h"

#include <glad/glad.h>

#include "system/MessageBox.h"

MainWindow::MainWindow(uint8_t pixelSize)
    : Window("Migraine", 64 * pixelSize, 32 * pixelSize) {

    // Load OpenGL. NEVER FORGET TO LOAD OpenGL
    if (!gladLoadGLLoader(((GLADloadproc) SDL_GL_GetProcAddress))) {
        MessageBox::error("Failed to load OpenGL");
        exit(1);
    }

    _c8 = new Chip8();
    _renderer = new Renderer(_c8);
}

MainWindow::~MainWindow() {
    delete _c8;
    delete _renderer;
    free();
}

void MainWindow::loadRom(const std::string &path) {
    _c8->unset();
    if (_c8->load(path)) {
        SDL_SetWindowTitle(_window, path.substr(path.find_last_of('/') + 1).c_str());
    }
}

void MainWindow::render() {
    makeCurrent();
    _renderer->render();
}

