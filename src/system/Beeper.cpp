#include "Beeper.h"
#include "MessageBox.h"

#include <algorithm>
#include <cmath>

#include <SDL2/SDL_timer.h>

#define PI2 6.28318530718

/**
 * @brief Function that generates the signal for the sound device to play
 * 
 * @param userdata Pointer to an AudioData object
 * @param s Pointer to the buffer supplied by SDL to write the signal to
 * @param len Size of the buffer in bytes
 * 
 * @note The AudioData object MUST LIVE AS LONG AS THE Beeper OBJECT.
*/
void audioCallback(void *userdata, uint8_t *s, int len) {
    if (!s) return;
    uint16_t *stream = (uint16_t *)s;
    AudioData *audio = (AudioData *)userdata;
    audio->volume = std::clamp(audio->volume, 0.f, 1.f);
    float time = 0.f;

    len /= sizeof(uint16_t);
    float amplitude = 10'000 * audio->volume;

    for (int i = 0; i < len; i++, audio->sampleNum++) {
        double time = (double)audio->sampleNum / 48'000.f;
        
        if (audio->type == SignalType::Square) {
            stream[i] = (uint16_t) (amplitude * (std::sin(PI2 * audio->freq * time) > 0 ? 1 : -1));
        }
        else if (audio->type == SignalType::Sin) {
            stream[i] = (uint16_t) (amplitude * std::sin(PI2 * audio->freq * time));
        }
    }
}

Beeper::Beeper(float freq, float volume) {
    _audioConfig = {
        .freq = freq,
        .volume = volume,
        .sampleNum = 0,
        .type = SignalType::Square
    };

    SDL_AudioSpec desired;
    SDL_zero(desired);
    desired = {
        .freq = 48'000,
        .format = AUDIO_S16SYS,
        .channels = 1,
        .samples = 512,
        .callback = audioCallback,
        .userdata = &_audioConfig
    };

    _device = SDL_OpenAudioDevice(nullptr, 0, &desired, nullptr, 0);
    if (!_device) {
        MessageBox::warning(SDL_GetError(), "Failed to open audio device");
        return;
    }
    _audioOn = false;
}

Beeper::~Beeper() {
    SDL_PauseAudioDevice(_device, 1);
    SDL_CloseAudioDevice(_device);
}

void Beeper::beep() {
    if (!_audioOn) {
        _audioOn = true;
        _timeout = SDL_GetTicks64() + 100; // 100ms after this moment
        SDL_PauseAudioDevice(_device, 0); // turn audio on
    }
}

void Beeper::unbeep() {
    if (_audioOn && (SDL_GetTicks64() > _timeout)) {
        _audioOn = false;
        SDL_PauseAudioDevice(_device, 1); // turn audio off
    }
}

void Beeper::setFrequency(float freq) {
    _audioConfig.freq = freq;
}

void Beeper::setVolume(float volume) {
    _audioConfig.volume = volume;
}

void Beeper::setSignaltype(SignalType type) {
    _audioConfig.type = type;
}

