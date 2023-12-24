#include "Emulator.h"
#include "system/MessageBox.h"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <slog/slog.h>

#include <fstream>
#include <algorithm>


Emulator::Emulator(const std::string &path, bool debugMode) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    _pixelSize = 10;
    _cyclesPerFrame = 10;
    _debugMode = debugMode;
    _onColor[0] = 255;  _offColor[0] = 0;
    _onColor[1] = 255;  _offColor[1] = 0;
    _onColor[2] = 255;  _offColor[2] = 0;

    _beeper = new Beeper();

    _loadConfig();

    _main  = new MainWindow(_pixelSize);
    _debug = new Debugger(_main->_c8, _beeper);
    
    if (!_debugMode) _debug->hide();

    _main->_renderer->setOnColor(_onColor[0], _onColor[1], _onColor[2]);
    _main->_renderer->setOffColor(_offColor[0], _offColor[1], _offColor[2]);

    if (path == "") {
        _debugMode = true;
    }
    else {
        _main->loadRom(path);
    }
}

Emulator::~Emulator() {
    delete _debug;
    delete _main;
    delete _beeper;
    SDL_Quit();
}

void Emulator::_loadConfig() {
    std::fstream configFile;
    configFile.open("config", std::ios::in);
    if(!configFile.is_open()) {
        MessageBox::warning(
            "Migraine will start with the default configuration",
            "Failed to load configuration file"
        );
    }

    std::string line, key, value;
    int lineNum = 0;

    while (getline(configFile, line)) {
        lineNum++;

        line = line.substr(0, line.find("#")); // remove comments
        line.erase(0, line.find_first_not_of(" \t")); // trim leading whitespaces
        if (line == "") continue;

        key = line.substr(0, line.find_first_of("=")); // get part before "=" sign
        key = key.substr(0, key.find_first_of(" \t")); // trim trailing whitespaces

        value = line.substr(line.find_first_of("=") + 1); // get part after "=" sign
        value.erase(0, value.find_first_not_of(" \t")); // trim leading whitespaces
        value = value.substr(0, value.find_first_of(" \t")); // trim trailing whitespaces
        if (value == "") {
            slog::warning("invalid value at line " + std::to_string(lineNum));
            slog::message("value is empty");
            continue;
        }

        if (key == "pixel_size") {
            try {
                _pixelSize = std::stoi(value);
            }
            catch (std::exception &e) {
                slog::warning("invalid value at line " + std::to_string(lineNum));
                slog::message(e.what());
            }
        }
        else if (key == "cycles_per_frame") {
            try {
                _cyclesPerFrame = std::stoi(value);
            }
            catch (std::exception &e) {
                slog::warning("invalid value at line " + std::to_string(lineNum));
                slog::message(e.what());
            }
        }
        else if (key == "sound_frequency") {
            try {
                float frequency = std::clamp(std::stof(value), 20.f, 20'000.f);
                _beeper->setFrequency(frequency);
            }
            catch (std::exception &e) {
                slog::warning("invalid value at line " + std::to_string(lineNum));
                slog::message(e.what());
            }
        }
        else if (key == "sound_volume") {
            try {
                float volume = std::clamp(std::stof(value), 0.f, 1.f);
                _beeper->setVolume(volume);
            }
            catch (std::exception &e) {
                slog::warning("invalid value at line " + std::to_string(lineNum));
                slog::message(e.what());
            }
        }
        else if (key == "sound_signal") {
            try {
                // sin = sin wave
                // triangle = triangle wave
                // anything else = square wave
                SignalType type = (value == "sin" ? SignalType::Sin
                                 : value == "triangle" ? SignalType::Triangle
                                 : SignalType::Square);
                
                _beeper->setSignaltype(type);
            }
            catch (std::exception &e) {
                slog::warning("invalid value at line " + std::to_string(lineNum));
                slog::message(e.what());
            }
        }
        else if (key == "lit_pixel") {
            try {
                if (value.size() == 6) {
                    uint8_t r, g, b;
                    r = std::stoi(value.substr(0, 2), 0, 16);
                    g = std::stoi(value.substr(2, 2), 0, 16);
                    b = std::stoi(value.substr(4, 2), 0, 16);

                    _onColor[0] = r; _onColor[1] = g; _onColor[2] = b;
                }
                else if (value.size() == 3) {
                    uint8_t r, g, b;
                    r = std::stoi(value.substr(0, 1) + value.substr(0, 1), 0, 16);
                    g = std::stoi(value.substr(1, 1) + value.substr(1, 1), 0, 16);
                    b = std::stoi(value.substr(2, 1) + value.substr(2, 1), 0, 16);

                    _onColor[0] = r; _onColor[1] = g; _onColor[2] = b;
                }
                else {
                    slog::warning("invalid value at line " + std::to_string(lineNum));
                    slog::message(
                        "color values must be in the format `rrggbb` of `rgb` "
                        "where r, g and b are hexadecimal values"
                    );
                }
            }
            catch (std::exception &e) {
                slog::warning("invalid value at line " + std::to_string(lineNum));
                slog::message(e.what());
            }
        }
        else if (key == "unlit_pixel") {
            try {
                if (value.size() == 6) {
                    uint8_t r, g, b;
                    r = std::stoi(value.substr(0, 2), 0, 16);
                    g = std::stoi(value.substr(2, 2), 0, 16);
                    b = std::stoi(value.substr(4, 2), 0, 16);

                    _offColor[0] = r; _offColor[1] = g; _offColor[2] = b;
                }
                else if (value.size() == 3) {
                    uint8_t r, g, b;
                    r = std::stoi(value.substr(0, 1) + value.substr(0, 1), 0, 16);
                    g = std::stoi(value.substr(1, 1) + value.substr(1, 1), 0, 16);
                    b = std::stoi(value.substr(2, 1) + value.substr(2, 1), 0, 16);

                    _offColor[0] = r; _offColor[1] = g; _offColor[2] = b;
                }
                else {
                    slog::warning("invalid value at line " + std::to_string(lineNum));
                    slog::message(
                        "color values must be in the format `rrggbb` of `rgb` "
                        "where r, g and b are hexadecimal values"
                    );
                }
            }
            catch (std::exception &e) {
                slog::warning("invalid value at line " + std::to_string(lineNum));
                slog::message(e.what());
            }
        }
        else {
            slog::warning("invalid key at line " + std::to_string(lineNum));
        }
    }
}

void Emulator::run() {
    bool running = true;
    SDL_Event event;

    // Main loop
    while (running) {
        // Event handling
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
                break;
            }

            if (event.type == SDL_USEREVENT && event.user.code == 0) {
                // when we get this event, we toggle on or off the crt effect
                _main->_renderer->crtEffect();
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_F12:
                    _debug->show();
                    break;
                }
            }
            else if (event.type == SDL_DROPFILE) {
                char *fileName = event.drop.file;
                _main->loadRom(fileName);
                SDL_free(fileName);
                _debugMode = false;
            }

            // handle window events
            _main->handleEvents(event);
            _debug->handleEvents(event);
            _debug->_gui.handleEvents(event);

            // close the app if all windows are closed
            if(!_main->isShown()) {
                SDL_Event tempEvent;
                tempEvent.type = SDL_QUIT;
                SDL_PushEvent(&tempEvent);
            }
        }

        if (!_debugMode) {
            for (int i = 0; i < _cyclesPerFrame; i++) {
                _main->_c8->emulateCycle();
            }
        }

        // turn off audio after some time
        _beeper->unbeep();

        // Decrement timers
        if (_main->_c8->delayTimer > 0) _main->_c8->delayTimer--;
        if (_main->_c8->soundTimer > 0) {
            _beeper->beep();
            _main->_c8->soundTimer--;
        }

        // Rendering
        _debug->render();
        _main->render();
        
        _main->swapBuffers();
        _debug->swapBuffers();
    }
    
}

