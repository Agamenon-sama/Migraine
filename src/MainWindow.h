#pragma once

#include "system/Window.h"
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
    MainWindow(uint8_t pixelSize);
    ~MainWindow();

    /**
     * @brief Loads a new rom to the emulator and sets the title appropriately
    */
    void loadRom(const std::string &path);

    /**
     * @brief Draws to the emulator window
    */
    void render();

    /**
     * @brief Owning pointer to chip8 emulator
    */
    Chip8 *_c8;

    /**
     * @brief Owning pointer to the renderer of the emulator window
    */
    Renderer *_renderer;
};
