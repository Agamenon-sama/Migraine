#include "MainWindow.h"

#include <glad/glad.h>

MainWindow::MainWindow(const std::string &path) : Window("Insert title later", 800, 600) {
    _c8 = new Chip8();
    _renderer = new Renderer(_window);
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
    _renderer->clear();
}

