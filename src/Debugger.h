#pragma once

#include "Window.h"
#include "GuiFrame.h"
#include "chip8.h"

#include <imgui/imgui_memory_editor.h>

class Debugger : public Window {
    public:
    Debugger(Chip8 *chip);
    ~Debugger();

    void render();

    GuiFrame _gui;
    MemoryEditor _memEditor;
    
    private:
    Chip8 *_chip;
};
