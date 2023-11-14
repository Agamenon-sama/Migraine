#pragma once

#include "chip8.h"
#include "Renderer.h"
#include "Window.h"
#include "Debugger.h"
#include "MainWindow.h"

class Emulator {
public:
    /**
     * @brief Constructor
     * 
     * @param path path to the rom file
     * @param debugMode if true, make it that we only execute an instruction after
     * the button was pressed in the Debugger window
    */
    Emulator(const std::string &path, bool debugMode = false);
    ~Emulator();

    /**
     * @brief Runs the emulator after setup
     * 
     * This function contains the main loop which handle window events and
     * renders to the screen
    */
    void run();

private:
    MainWindow *_main; ///< @brief Pointer to the emulator screen window and emulator
    Debugger *_debug; ///< @brief Pointer to the debugger window

    /**
     * @brief Flag which specifies whether we let the emulator run on its own
     * or only run an instruction on user demand
    */
    bool _debugMode; 
};
