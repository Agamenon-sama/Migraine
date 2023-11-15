#include "Emulator.h"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <slog/slog.h>

#include <fstream>


Emulator::Emulator(const std::string &path, bool debugMode) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    _pixelSize = 10;
    _cyclesPerFrame = 10;
    _debugMode = debugMode;

    _loadConfig();

    _main  = new MainWindow(path, _pixelSize);
    _debug = new Debugger(_main->_c8);
}

Emulator::~Emulator() {
    delete _debug;
    delete _main;
    SDL_Quit();
}

void Emulator::_loadConfig() {
    std::fstream configFile;
    configFile.open("config", std::ios::in);
    if(!configFile.is_open()) {
        std::cerr << "Failed to load configuration file\n";
        std::cerr << "Migraine will start with the default configuration\n";
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

            // handle window events
            _main->handleEvents(event);
            _debug->handleEvents(event);
            _debug->_gui.handleEvents(event);

            // close the app if all windows are closed
            if(!_main->isShown() && !_debug->isShown()) {
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

        // Rendering
        _debug->render();
        _main->render();
        
        _main->swapBuffers();
        _debug->swapBuffers();
    }
    
}

