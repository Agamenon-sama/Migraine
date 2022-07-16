#pragma once

#include <SDL2/SDL.h>
#include <string>

class Window {
    public:
        Window(std::string title, int width, int height, bool resizable = false);
        ~Window();

        bool init();
        void handleEvents(SDL_Event &ev);
        void free();
        void createGLContext();
        void swapBuffers();
        void focus();


        SDL_Window *getSDLWindow() const;
        SDL_GLContext getGLContext() const;
        int getWidth() const;
        int getHeight() const;
        uint32_t getFullscreenMode() const;
        void setFullscreenMode(uint32_t mode);
        bool hasMouseFocus() const;
        bool hasKeyboardFocus() const;
        bool isMinimized() const;
        bool isShown() const;

    protected:
        SDL_Window *_window;
        SDL_GLContext _glContext;
        uint8_t _windowID;

        int _width;
        int _height;

        std::string _title;
        bool _mouseFocus;
        bool _keyboardFocus;
        bool _fullscreen;
        bool _minimized;
        bool _shown;
        uint32_t _fullscreenMode; // Holds the fullscreen flag
};

