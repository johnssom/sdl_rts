#ifndef BUTTON_H
#define BUTTON_H

#include <string>
#include <SDL2/SDL.h>

#include "sprite.h"

class Button {
    private:
        Sprite* _sprite;
        int _x, _y;
        int _w, _h;

    public:
        Button(SDL_Renderer* renderer, std::string filePath, int x, int y, int w, int h);
        int getX() const;
        int getY() const;
        int getWidth() const;
        int getHeight() const;
        void setPosition(int x, int y);
        void setDimensions(int w, int h);
        bool containsPoint(int px, int py) const;
        void render(SDL_Renderer* renderer);
        ~Button();
};

#endif
