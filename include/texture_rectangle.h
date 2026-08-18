#ifndef TEXTURE_RECTANGLE_H
#define TEXTURE_RECTANGLE_H

#include <string>
#include <SDL2/SDL.h>

#include "resource_manager.h"

class TextureRectangle {
    private:
        SDL_Rect _boundingBox;
        SDL_Texture* _texture;
        inline const SDL_Rect& getBoundingBox() const { return _boundingBox; }
    
    public:
        TextureRectangle(SDL_Renderer* renderer, std::string filePath);
        TextureRectangle(SDL_Renderer* renderer, std::string filePath, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue);
        int getPositionX();
        int getPositionY();
        int getWidth();
        int getHeight();
        void setPosition(int x, int y);
        void setDimensions(int w, int h);
        void update();
        void render(SDL_Renderer* renderer);
        ~TextureRectangle();
};

#endif