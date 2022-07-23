#include "Debugger.h"

#include <glad/glad.h>

#include <iostream>

Debugger::Debugger(Chip8 *chip) : Window("Debugger", 600, 700, true) {
    // createGLContext();
    _gui.create(this);

    _chip = chip;
    // char *mess = "Chams, you're the worst programmer that has ever lived";
    // memcpy(_data, mess, 55);
}

Debugger::~Debugger() {
    _gui.shutdown();
}

void Debugger::render() {
    SDL_GL_MakeCurrent(_window, _glContext);
    _gui.beginRender();

    // draw memory viewer in a seperate frame
    _memEditor.DrawWindow("Memory Viewer", _chip->mem, 1024);

    ImGui::Begin("Debugger");

    // Init copies of registers
    int stateRegs[6] = {_chip->pc, _chip->sp, _chip->i, _chip->opcode, _chip->delayTimer, _chip->soundTimer};
    int vRegs[0xf];
    for (int i = 0; i < 0xf; i++) {
        vRegs[i] = _chip->v[i];
    }

    ImGui::Text("State Registers");
    ImGui::InputInt("program counter", &stateRegs[0]);
    ImGui::InputInt("stack pointer", &stateRegs[1]);
    ImGui::InputInt("address register", &stateRegs[2]);
    ImGui::InputInt("current opcode", &stateRegs[3]);
    ImGui::InputInt("delay timer", &stateRegs[4]);
    ImGui::InputInt("sound timer", &stateRegs[5]);

    ImGui::Text("V Registers");
    if (ImGui::BeginTable("split", 4)) {
        ImGui::TableNextColumn(); ImGui::InputInt("v0", &vRegs[0x0]);
        ImGui::TableNextColumn(); ImGui::InputInt("v1", &vRegs[0x1]);
        ImGui::TableNextColumn(); ImGui::InputInt("v2", &vRegs[0x2]);
        ImGui::TableNextColumn(); ImGui::InputInt("v3", &vRegs[0x3]);
        ImGui::TableNextColumn(); ImGui::InputInt("v4", &vRegs[0x4]);
        ImGui::TableNextColumn(); ImGui::InputInt("v5", &vRegs[0x5]);
        ImGui::TableNextColumn(); ImGui::InputInt("v6", &vRegs[0x6]);
        ImGui::TableNextColumn(); ImGui::InputInt("v7", &vRegs[0x7]);
        ImGui::TableNextColumn(); ImGui::InputInt("v8", &vRegs[0x8]);
        ImGui::TableNextColumn(); ImGui::InputInt("v9", &vRegs[0x9]);
        ImGui::TableNextColumn(); ImGui::InputInt("vA", &vRegs[0xa]);
        ImGui::TableNextColumn(); ImGui::InputInt("vB", &vRegs[0xb]);
        ImGui::TableNextColumn(); ImGui::InputInt("vC", &vRegs[0xc]);
        ImGui::TableNextColumn(); ImGui::InputInt("vD", &vRegs[0xd]);
        ImGui::TableNextColumn(); ImGui::InputInt("vE", &vRegs[0xe]);
        ImGui::TableNextColumn(); ImGui::InputInt("vF", &vRegs[0xf]);
        ImGui::EndTable();
    }
    
    if (ImGui::Button("Emulate cycle")) {
        _chip->emulateCycle();
    }

    if (ImGui::Button("reset")) {
        _chip->reset();
    }

    ImGui::End();

    _gui.endRender();
}
