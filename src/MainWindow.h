#pragma once

#include "Window.h"
#include "chip8.h"
#include "Renderer.h"

class MainWindow : public Window {
    public:
    MainWindow(const std::string &path);
    ~MainWindow();

    void render();

    Chip8 *_c8;

    private:
    Renderer *_renderer;
};
