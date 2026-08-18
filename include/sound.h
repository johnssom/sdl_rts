#ifndef SOUND_H
#define SOUND_H

#include <string>

#include <SDL2/SDL.h>

class ISound {
    public:
        virtual ~ISound() {};
        virtual void play() = 0;
        virtual void stop() = 0;
};

class Sound : public ISound {
    private:
        SDL_AudioDeviceID _device;
        SDL_AudioSpec _audioSpec;
        Uint8* _waveStart;
        Uint32 _waveLength;
    public:
        Sound(std::string filePath);
        void play();
        void stop();
        void setupDevice();
        ~Sound();
};

#endif