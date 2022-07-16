#pragma once

#include "chip8.h"
#include "Renderer.h"
#include "Window.h"
#include "Debugger.h"

class Emulator {
    public:
        Emulator(const std::string &path);
        ~Emulator();

        void run();

    private:
        Window *_window1;
        // Window *_window2;
        Debugger *_debug;
        Chip8 _c8;
        // Renderer _renderer;
};
