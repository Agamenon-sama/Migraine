#pragma once

#include "chip8.h"
#include "Renderer.h"
#include "Debugger.h"
#include "MainWindow.h"
#include "system/Beeper.h"

/**
 * @brief Handles the whole emulator
*/
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

    Beeper *_beeper; ///< @brief Pointer to system that handles sound

    /**
     * @brief Flag which specifies whether we let the emulator run on its own
     * or only run an instruction on user demand
    */
    bool _pause;

    uint8_t _pixelSize; ///< @brief Width and height of the pixel on screen
    /**
     * @brief Number of fetch-decode-execute operations to do per frame
     * 
     * By default, VSync is on, meaning the renderer should draw at a frame rate
     * equal to your monitor's refresh rate. In every frame, the emulator will execute
     * a number of instructions from the rom file equal to the value set in this variable
    */
    uint16_t _cyclesPerFrame;

    uint8_t _onColor[3];  ///< @brief RGB color of lit pixels
    uint8_t _offColor[3]; ///< @brief RGB color of unlit pixels

    /**
     * @brief Reads the configuration file and sets the emulator accordingly
     * 
     * The configuration file must be called "config" and be placed in the same
     * folder as the executable
    */
    void _loadConfig();
};
