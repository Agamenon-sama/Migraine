#include "Debugger.h"

#include <glad/glad.h>

#include <iostream>
#include <fstream>

Debugger::Debugger(Chip8 *chip) : Window("Debugger", 600, 700, true) {
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
    ImGui::InputInt("program counter", &stateRegs[0], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("stack pointer", &stateRegs[1], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("address register", &stateRegs[2], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("current opcode", &stateRegs[3], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("delay timer", &stateRegs[4], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("sound timer", &stateRegs[5], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);

    ImGui::Text("V Registers");
    if (ImGui::BeginTable("split", 4)) {
        ImGui::TableNextColumn(); ImGui::InputInt("v0", &vRegs[0x0]);
        ImGui::TableNextColumn(); ImGui::InputInt("v4", &vRegs[0x4]);
        ImGui::TableNextColumn(); ImGui::InputInt("v8", &vRegs[0x8]);
        ImGui::TableNextColumn(); ImGui::InputInt("vC", &vRegs[0xc]);
        ImGui::TableNextColumn(); ImGui::InputInt("v1", &vRegs[0x1]);
        ImGui::TableNextColumn(); ImGui::InputInt("v5", &vRegs[0x5]);
        ImGui::TableNextColumn(); ImGui::InputInt("v9", &vRegs[0x9]);
        ImGui::TableNextColumn(); ImGui::InputInt("vD", &vRegs[0xd]);
        ImGui::TableNextColumn(); ImGui::InputInt("v2", &vRegs[0x2]);
        ImGui::TableNextColumn(); ImGui::InputInt("v6", &vRegs[0x6]);
        ImGui::TableNextColumn(); ImGui::InputInt("vA", &vRegs[0xa]);
        ImGui::TableNextColumn(); ImGui::InputInt("vE", &vRegs[0xe]);
        ImGui::TableNextColumn(); ImGui::InputInt("v3", &vRegs[0x3]);
        ImGui::TableNextColumn(); ImGui::InputInt("v7", &vRegs[0x7]);
        ImGui::TableNextColumn(); ImGui::InputInt("vB", &vRegs[0xb]);
        ImGui::TableNextColumn(); ImGui::InputInt("vF", &vRegs[0xf]);
        ImGui::EndTable();
    }
    

    ImGui::Text("Controls");
    if (ImGui::Button("Emulate cycle")) {
        _chip->emulateCycle();
    }

    if (ImGui::Button("reset")) { _chip->reset(); } ImGui::SameLine();
    if (ImGui::Button("unset")) { _chip->unset(); }

    ImGui::End();

    ImGui::Begin("Disassembly");

    char filename[64] = "";
    ImGui::InputText("file name", filename, 64); // FIXME: this is not reading the file name

    if (ImGui::Button("disassemble rom")) {
        _assemblyCode = _disassembler.disassemble(_chip->mem);
    } ImGui::SameLine();
    if (ImGui::Button("save disassembled code")) {
        std::fstream file(filename, std::ios::out);
        if (!file.is_open()) {
            _assemblyCode = "Failed to write the file " + std::string(filename);
        }

        file.write(_assemblyCode.c_str(), _assemblyCode.size());
    } ImGui::SameLine();
    if (ImGui::Button("load assembly source")) {
        std::fstream file(filename, std::ios::in);
        if (!file.is_open()) {
            _assemblyCode = "Failed to read the file " + std::string(filename);
        }

        file.seekg(0, std::ios::end);
        int size = file.tellg();
        file.seekg(0);

        char *buff = new char[size];
        file.read(buff, size);
        _assemblyCode = buff;
        delete [] buff;
    }

    ImGui::TextWrapped("%s", _assemblyCode.c_str());

    ImGui::End();

    _gui.endRender();
}
