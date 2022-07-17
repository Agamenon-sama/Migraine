#include "Emulator.h"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_memory_editor.h>


Emulator::Emulator(const std::string &path) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    _main  = new MainWindow(path);
    _debug = new Debugger(_main->_c8->mem);

    // Load OpenGL. NEVER FORGET TO LOAD OpenGL
    if (!gladLoadGLLoader(((GLADloadproc) SDL_GL_GetProcAddress))) {
        std::cerr << "Failed to load OpenGL\n";
        exit(1);
    }
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

        // Rendering
        _debug->render();
        _main->render();
        
        _main->swapBuffers();
        _debug->swapBuffers();
    }
    
}

