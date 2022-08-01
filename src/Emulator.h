#pragma once

#include "chip8.h"
#include "Renderer.h"
#include "Window.h"
#include "Debugger.h"
#include "MainWindow.h"

class Emulator {
    public:
        Emulator(const std::string &path, bool debugMode = false);
        ~Emulator();

        void run();

    private:
        MainWindow *_main;
        Debugger *_debug;

        bool _debugMode;
};
