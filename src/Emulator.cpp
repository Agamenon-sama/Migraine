#include "Emulator.h"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


Emulator::Emulator(const std::string &path, bool debugMode) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    _main  = new MainWindow(path);
    _debug = new Debugger(_main->_c8);
    _debugMode = debugMode;
}

Emulator::~Emulator() {
    delete _debug;
    delete _main;
    SDL_Quit();
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
            _main->_c8->emulateCycle();
        }

        // Rendering
        _debug->render();
        _main->render();
        
        _main->swapBuffers();
        _debug->swapBuffers();
    }
    
}

