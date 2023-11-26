#pragma once

#include <SDL2/SDL_audio.h>

/**
 * @brief Specify the formula used to create the wave form of the sound
*/
enum class SignalType : int {
    Square = 0,
    Sin = 1,
    Triangle = 2
};

/**
 * @brief Contains the data sent to the callback for sound generation
*/
struct AudioData {
    float freq; ///< @brief Frequency of the note to be played
    float volume; ///< @brief Volume of the sound between 0 and 1.0
    /**
     * @brief Used internally for sampling the signal
     * 
     * @note Not meant to be set by user and should be treated as private
    */
    int sampleNum;
    SignalType type; ///< @brief Type of the signal
};

/**
 * @brief Provides all the sound requirements of the program
*/
class Beeper {
public:
    /**
     * @brief Constructor
     * 
     * @param freq frequency of the note to be played
     * @param volume volume of the sound between 0 and 1.0
    */
    Beeper(float freq = 440.f, float volume = 0.3f);
    ~Beeper();

    /**
     * @brief Starts playing sound
     * 
     * Plays a sound for around 100ms
    */
    void beep();
    /**
     * @brief Pauses the sound if timeout is over
     * 
     * This function must always be called along with beep() to check
     * if we passed the timeout. If the timeout isn't over, the sound will continue
    */
    void unbeep();

    /**
     * @brief Sets a new frequency
     * 
     * @param freq the new frequency
    */
    void setFrequency(float freq);
    /**
     * @brief Sets a new volume
     * 
     * @param volume the new volume
    */
    void setVolume(float volume);
    /**
     * @brief Sets a new signal type
     * 
     * @param type the new signal type
    */
    void setSignaltype(SignalType type);

    float getFrequency() const { return _audioConfig.freq; } ///< @brief Gets the current frequency
    float getVolume() const { return _audioConfig.volume; } ///< @brief Gets the current volume
    SignalType getSignalType() const { return _audioConfig.type; } ///< @brief Gets the current signal type

private:
    SDL_AudioDeviceID _device; ///< @brief Sound device id
    AudioData _audioConfig; ///< @brief Contains the data sent to the callback for sound generation

    uint32_t _timeout; ///< @brief Indicates the moment after which we pause the sound
    bool _audioOn; ///< @brief Indicates if sound is on
};
