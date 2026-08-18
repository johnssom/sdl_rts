#ifndef ANIMATED_SPRITE_H
#define ANIMATED_SPRITE_H

#include <string>
#include <SDL2/SDL.h>

#include "resource_manager.h"

class AnimatedSprite {
    private:
        SDL_Rect _spriteSheet;
        SDL_Rect _boundingBox;
        SDL_Texture* _texture;

    public:
        AnimatedSprite(SDL_Renderer* renderer, std::string filePath);
        void setBoundingBox(int x, int y, int w, int h);
        void playFrame(int x, int y, int w, int h, int frame);
        void update();
        void render(SDL_Renderer* renderer);
        ~AnimatedSprite();
};

#endif