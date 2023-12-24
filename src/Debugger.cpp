#include "Debugger.h"

#include <glad/glad.h>
#include <SDL2/SDL_events.h>

#include <fstream>

Debugger::Debugger(Chip8 *chip, Beeper *beeper) : Window("Debugger", 1280, 720, true) {
    _gui.create(this);

    _chip = chip;
    _beeper = beeper;
}

Debugger::~Debugger() {
    _gui.shutdown();
}

void Debugger::handleEvents(SDL_Event &ev) {
    Window::handleEvents(ev);

    if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_F11) {
        if(_fullscreen) {
            SDL_SetWindowFullscreen(_window, 0);
            _fullscreen = false;
        } else {
            SDL_SetWindowFullscreen(_window, _fullscreenMode);
            _fullscreen = true;
            _minimized = false;
        }
    }
}

static bool inputInt(const char* label, int* v) {
    int step = 1, step_fast = 100;
    ImGuiInputTextFlags flags = 0;
    return ImGui::InputScalar(
        label, ImGuiDataType_S32, (void*)v, (void*)(step > 0 ? &step : NULL),
        (void*)(step_fast > 0 ? &step_fast : NULL), "%02X", flags
    );
}

void Debugger::render() {
    SDL_GL_MakeCurrent(_window, _glContext);
    _gui.beginRender();

    // draw memory viewer in a seperate frame
    _memEditor.DrawWindow("Memory Viewer", _chip->mem, 4096);

    ImGui::Begin("Debugger");

    // Init copies of registers
    int stateRegs[6] = {_chip->pc, _chip->sp, _chip->i, _chip->opcode, _chip->delayTimer, _chip->soundTimer};
    int vRegs[0x10];
    for (int i = 0; i < 0x10; i++) {
        vRegs[i] = _chip->v[i];
    }

    ImGui::Text("State Registers");
    ImGui::InputInt("program counter", &stateRegs[0], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("stack pointer", &stateRegs[1], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("address register", &stateRegs[2], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("last opcode", &stateRegs[3], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("delay timer", &stateRegs[4], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("sound timer", &stateRegs[5], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);

    ImGui::Text("V Registers");
    if (ImGui::BeginTable("split", 4)) {
        ImGui::TableNextColumn(); inputInt("v0", &vRegs[0x0]);
        ImGui::TableNextColumn(); inputInt("v4", &vRegs[0x4]);
        ImGui::TableNextColumn(); inputInt("v8", &vRegs[0x8]);
        ImGui::TableNextColumn(); inputInt("vC", &vRegs[0xc]);
        ImGui::TableNextColumn(); inputInt("v1", &vRegs[0x1]);
        ImGui::TableNextColumn(); inputInt("v5", &vRegs[0x5]);
        ImGui::TableNextColumn(); inputInt("v9", &vRegs[0x9]);
        ImGui::TableNextColumn(); inputInt("vD", &vRegs[0xd]);
        ImGui::TableNextColumn(); inputInt("v2", &vRegs[0x2]);
        ImGui::TableNextColumn(); inputInt("v6", &vRegs[0x6]);
        ImGui::TableNextColumn(); inputInt("vA", &vRegs[0xa]);
        ImGui::TableNextColumn(); inputInt("vE", &vRegs[0xe]);
        ImGui::TableNextColumn(); inputInt("v3", &vRegs[0x3]);
        ImGui::TableNextColumn(); inputInt("v7", &vRegs[0x7]);
        ImGui::TableNextColumn(); inputInt("vB", &vRegs[0xb]);
        ImGui::TableNextColumn(); inputInt("vF", &vRegs[0xf]);
        ImGui::EndTable();
    }
    

    ImGui::Text("Controls");
    if (ImGui::Button("Emulate cycle")) {
        if (_chip->pause) {
            _chip->pause = false;
        }
        _chip->emulateCycle();
    }

    if (ImGui::Button("pause")) {
        SDL_Event ev;
        ev.type = SDL_USEREVENT;
        ev.user.code = 1;
        SDL_PushEvent(&ev);
    } ImGui::SameLine();
    if (ImGui::Button("resume")) {
        SDL_Event ev;
        ev.type = SDL_USEREVENT;
        ev.user.code = 2;
        SDL_PushEvent(&ev);
        if (_chip->pause) {
            _chip->pause = false;
        }
    } ImGui::SameLine();
    if (ImGui::Button("restart")) { _chip->reset(); } ImGui::SameLine();
    if (ImGui::Button("unset")) { _chip->unset(); }

    ImGui::InputInt("break point address", &_chip->breakPointAddress,
        1, 100, ImGuiInputTextFlags_CharsHexadecimal); ImGui::SameLine();

    if (_chip->pause) {
        // when we reach the break point address, we pause
        SDL_Event ev;
        ev.type = SDL_USEREVENT;
        ev.user.code = 1;
        SDL_PushEvent(&ev);
    }

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

    ImGui::Begin("Graphics & Sound");

    if (ImGui::Button("CRT effect")) {
        SDL_Event ev;
        ev.type = SDL_USEREVENT;
        ev.user.code = 0;
        SDL_PushEvent(&ev);
    }

    ImGui::Separator();
    
    float frequency = _beeper->getFrequency(), volume = _beeper->getVolume();
    int signalType = (int)_beeper->getSignalType();

    if (ImGui::InputFloat("Frequency", &frequency, 0.1f, 50.f, "%.2f")) {
        _beeper->setFrequency(frequency);
    }
    if (ImGui::SliderFloat("Volume", &volume, 0.f, 1.f)) {
        _beeper->setVolume(volume);
    }
    if (ImGui::Combo("Signal type", &signalType, "Square\0Sin\0Triangle\0\0")) {
        _beeper->setSignaltype((SignalType)signalType);
    }

    ImGui::End();

    _gui.endRender();
}
