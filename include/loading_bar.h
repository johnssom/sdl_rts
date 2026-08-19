#ifndef LOADING_BAR_H
#define LOADING_BAR_H

#include <SDL2/SDL.h>

class LoadingBar {
    private:
        float _x, _y;
        float _w, _h;
        SDL_Color _bgColor;
        SDL_Color _fillColor;

    public:
        LoadingBar(float x, float y, float w, float h);
        void setPosition(float x, float y);
        void setColors(SDL_Color bg, SDL_Color fill);
        void render(SDL_Renderer* renderer, float progress) const;
};

#endif
