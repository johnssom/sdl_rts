#ifndef DYNAMIC_TEXT_H
#define DYNAMIC_TEXT_H

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class DynamicText {
    private:
        static bool _ttfInitialized;
        TTF_Font* _font;
        SDL_Rect _boundingBox;
        SDL_Texture* _texture;
        SDL_Surface* _surface;
    public:
        DynamicText(std::string filePath, int pointSize);
        void render(SDL_Renderer* renderer, std::string text, int x, int y, int w, int h);
        ~DynamicText();
};

#endif