#include "GuiFrame.h"

// #include <GL/gl.h>
#include <glad/glad.h>
#include <SDL2/SDL_opengl.h>

bool GuiFrame::create(Window *window) {
    if(!window) {
        return false;
    }
    
    IMGUI_CHECKVERSION();
    
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;

    ImGui_ImplSDL2_InitForOpenGL(window->getSDLWindow(), window->getGLContext());
    ImGui_ImplOpenGL3_Init("#version 330 core");

    ImGui::StyleColorsDark();

    _parentWindow = window;

    return true;
}

void GuiFrame::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void GuiFrame::handleEvents(SDL_Event &event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void GuiFrame::beginRender() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(_parentWindow->getSDLWindow());
    ImGui::NewFrame();
}

void GuiFrame::endRender() {
    ImGui::Render();
    ImGuiIO &io = ImGui::GetIO();
    // glViewport(0, 0, 800, 600);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.11, 0.11, 0.11, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
