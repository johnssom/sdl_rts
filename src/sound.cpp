#include "sound.h"

#include <iostream>

Sound::Sound(std::string filePath) {
    if (SDL_LoadWAV(filePath.c_str(), &_audioSpec, &_waveStart, &_waveLength) == nullptr) {
        std::cerr << "Failed to load sound. Error (" << SDL_GetError() << ")\n";        
    }
}

void Sound::play() {
    if (_waveStart == nullptr || _device == 0) {
        std::cerr << "No audio data or device\n";
        return;
    }

    int status = SDL_QueueAudio(_device, _waveStart, _waveLength);
    if (_waveStart == nullptr) {
        std::cerr << "SDL_LoadWAV failed: " << SDL_GetError() << "\n";
    }
    if (status != 0) {
        std::cerr << "SDL_QueueAudio failed: " << SDL_GetError() << "\n";
    }
    SDL_PauseAudioDevice(_device, 0);
}

void Sound::stop() {
    SDL_PauseAudioDevice(_device, 1);
}

void Sound::setupDevice() {
    _device = SDL_OpenAudioDevice(nullptr, 0, &_audioSpec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (0 == _device) {
        std::cerr << "Failed to open audio device. Error (" << SDL_GetError() << ")\n";
    }
}

Sound::~Sound() {
    SDL_FreeWAV(_waveStart);
    SDL_CloseAudioDevice(_device);    
}