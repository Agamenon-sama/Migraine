#pragma once

#include "Window.h"
#include "chip8.h"
#include "Renderer.h"

/**
 * @brief Class that holds the emulator and emulator window and renderer
*/
class MainWindow : public Window {
public:
    /**
     * @brief Constructor
     * 
     *  @param path path to the rom file to load
    */
    MainWindow(const std::string &path);
    ~MainWindow();

    /**
     * @brief Draws to the emulator window
    */
    void render();

    /**
     * @brief Owning pointer to chip8 emulator
    */
    Chip8 *_c8;

private:
    /**
     * @brief Owning pointer to the renderer of the emulator window
    */
    Renderer *_renderer;
};
