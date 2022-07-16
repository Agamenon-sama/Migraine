#include "Emulator.h"

#include <glad/glad.h>
// #include <GL/gl.h>
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

    _window1 = new Window("First window", 800, 600);
    // _window2 = new Window("Second window", 400, 300);
    _c8.load(path);
    _debug = new Debugger(_c8.mem);
    std::cout << "Finished constractor of Emulator\n";
}

Emulator::~Emulator() {
    // delete _window2;
    delete _debug;
    delete _window1;
    SDL_Quit();
}

void Emulator::run() {
    std::cout << "running the emulator\n";
    bool running = true;
    SDL_Event event;

    // Main loop
    while (running) {
        // std::cout << "In main loop\n";
        // Event handling
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
                break;
            }

            // handle window events
            _window1->handleEvents(event);
            // _window2->handleEvents(event);
            _debug->handleEvents(event);
            _debug->_gui.handleEvents(event);

            // close the app if all windows are closed
            // if(!_window1->isShown() && !_window2->isShown()) {
            if(!_window1->isShown() && !_debug->isShown()) {
            // if(!_debug->isShown()) {
                std::cout << "Exiting\n";
                SDL_Event tempEvent;
                tempEvent.type = SDL_QUIT;
                SDL_PushEvent(&tempEvent);
            }
        }

        // Rendering
        _debug->render();
        
    }
    
}

