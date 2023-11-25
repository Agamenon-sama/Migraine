#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include <SDL2/SDL.h>

#include "system/Window.h"

class GuiFrame {
    public:
        GuiFrame() = default;
        ~GuiFrame() = default;

        bool create(Window *window);
        void shutdown();

        void handleEvents(SDL_Event &event);

        void beginRender();
        void endRender();

    private:
        Window *_parentWindow;
};
