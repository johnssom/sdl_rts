#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>

class Sprite {
    public:
        virtual int getPositionX() = 0;
        virtual int getPositionY() = 0;
        virtual int getWidth() = 0;
        virtual int getHeight() = 0;
        virtual void setPosition(int x, int y) = 0;
        virtual void setDimensions(int w, int h) = 0;
        virtual void update() = 0;
        virtual void render(SDL_Renderer* renderer) = 0;
        virtual ~Sprite() {}
};

#endif
