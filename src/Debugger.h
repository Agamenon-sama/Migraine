#pragma once

#include "Window.h"
#include "GuiFrame.h"

#include <imgui/imgui_memory_editor.h>

class Debugger : public Window {
    public:
    Debugger(uint8_t *data);
    ~Debugger();

    void render();

    GuiFrame _gui;
    MemoryEditor _memEditor;
    
    private:
    uint8_t *_data;
};
