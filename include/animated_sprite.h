#ifndef ANIMATED_SPRITE_H
#define ANIMATED_SPRITE_H

#include <string>
#include <SDL2/SDL.h>

#include "sprite.h"
#include "resource_manager.h"

class AnimatedSprite : public Sprite {
    private:
        SDL_Rect _spriteSheet;
        SDL_Rect _boundingBox;
        SDL_Texture* _texture;

    public:
        AnimatedSprite(SDL_Renderer* renderer, std::string filePath);
        void playFrame(int x, int y, int w, int h, int frame);
        int getPositionX() override;
        int getPositionY() override;
        int getWidth() override;
        int getHeight() override;
        void setPosition(int x, int y) override;
        void setDimensions(int w, int h) override;
        void update() override;
        void render(SDL_Renderer* renderer) override;
        ~AnimatedSprite();
};

#endif