#include "Window.h"

#include <iostream>
#include <sstream>

#include <glad/glad.h>
#include <SDL2/SDL_opengl.h>

Window::Window(std::string title, int width, int height, bool resizable) {
    uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    if (resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }
    
    _window = SDL_CreateWindow(title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height, flags);

    if(!_window) {
        std::cerr << "Failed to create window : " << SDL_GetError() << "\nExiting\n";
        exit(1);
    }

    _windowID = SDL_GetWindowID(_window);

    createGLContext();

    _width = width;
    _height = height;
    _title = title;
    _mouseFocus = true;
    _keyboardFocus = true;
    _fullscreen = false;
    _minimized = true;
    _shown = true;
    _fullscreenMode = SDL_WINDOW_FULLSCREEN_DESKTOP;
}

Window::~Window() {
    free();
}

void Window::handleEvents(SDL_Event &ev) {
    if( ev.type == SDL_WINDOWEVENT && ev.window.windowID == _windowID) {
        bool updateCaption = false;
        switch (ev.window.event) {
            case SDL_WINDOWEVENT_SHOWN:
                _shown = true;
                break;
            case SDL_WINDOWEVENT_HIDDEN:
                _shown = false;
                break;
            case SDL_WINDOWEVENT_SIZE_CHANGED: // changing window size
                _width = ev.window.data1;
                _height = ev.window.data2;
                swapBuffers(); // Experimental
                break;
            case SDL_WINDOWEVENT_EXPOSED:
                swapBuffers(); // Experimental
                break;
            case SDL_WINDOWEVENT_ENTER: // Mouse on the window
                _mouseFocus = true;
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_LEAVE: // Mouse off the window
                _mouseFocus = false;
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                _keyboardFocus = true;
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                _keyboardFocus = false;
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                _minimized = true;
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
                _minimized = false;
                break;
            case SDL_WINDOWEVENT_CLOSE: // X button pressed
                SDL_HideWindow(_window);
                break;

            default:
                break;
        }
        if(updateCaption) { // This is temporary
            std::stringstream caption;
            caption << _title << " - Mouse " << (_mouseFocus ? "ON" : "OFF") << " KeyBoard " << (_keyboardFocus ? "ON" : "OFF");
            SDL_SetWindowTitle(_window, caption.str().c_str());
        }
    } else if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_F11) {
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

void Window::free() {
    SDL_DestroyWindow(_window);
    _width = 0;
    _height = 0;
    SDL_GL_DeleteContext(_glContext);
}

void Window::makeCurrent() {
    SDL_GL_MakeCurrent(_window, _glContext);
}

void Window::swapBuffers() {
    makeCurrent();
    SDL_GL_SwapWindow(_window);
}

void Window::focus() {
    if(!_shown) { // if window is not shown, we start by showing it
        SDL_ShowWindow(_window);
    }
    SDL_RaiseWindow(_window);
}

void Window::createGLContext() {
    _glContext = SDL_GL_CreateContext(_window);
}


SDL_Window *Window::getSDLWindow() const { return _window; }
SDL_GLContext Window::getGLContext() const { return _glContext; }

int Window::getWidth() const { return _width; }
int Window::getHeight() const { return _height; }
bool Window::hasMouseFocus() const { return _mouseFocus; }
bool Window::hasKeyboardFocus() const { return _keyboardFocus; }
bool Window::isMinimized() const { return _minimized; }
bool Window::isShown() const { return _shown; }
uint32_t Window::getFullscreenMode() const { return _fullscreenMode; }


void Window::setFullscreenMode(uint32_t mode) {
    /**
     * Sets the fullscreen mode
    */
    if(mode == SDL_WINDOW_FULLSCREEN || mode == SDL_WINDOW_FULLSCREEN_DESKTOP){
        _fullscreenMode = mode;
    }
}
