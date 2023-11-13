#include "MainWindow.h"

#include <glad/glad.h>

MainWindow::MainWindow(const std::string &path) : Window("Insert title later", 640, 320) {
    // Load OpenGL. NEVER FORGET TO LOAD OpenGL
    if (!gladLoadGLLoader(((GLADloadproc) SDL_GL_GetProcAddress))) {
        std::cerr << "Failed to load OpenGL\n";
        exit(1);
    }

    _c8 = new Chip8();
    _renderer = new Renderer(_window, _c8);
    _c8->load(path);
}

MainWindow::~MainWindow() {
    delete _c8;
    delete _renderer;
    free();
}


void MainWindow::render() {
    // Normally I should make calls for Renderer
    makeCurrent();
    _renderer->render();
}

