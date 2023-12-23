#pragma once

#include "system/Window.h"
#include "GuiFrame.h"
#include "chip8.h"
#include "Assembler/Disassembler.h"
#include "system/Beeper.h"

#include <imgui/imgui_memory_editor.h>

class Debugger : public Window {
public:
    Debugger(Chip8 *chip, Beeper *beeper);
    ~Debugger();

    void render();
    void handleEvents(SDL_Event &ev) override;
    GuiFrame _gui;
    MemoryEditor _memEditor;
    Disassembler _disassembler;
    std::string _assemblyCode;
    
private:
    Chip8 *_chip;
    Beeper *_beeper;
};
