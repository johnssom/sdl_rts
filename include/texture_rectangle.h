#ifndef TEXTURE_RECTANGLE_H
#define TEXTURE_RECTANGLE_H

#include <string>
#include <SDL2/SDL.h>

#include "sprite.h"
#include "resource_manager.h"

class TextureRectangle : public Sprite {
    private:
        SDL_Rect _boundingBox;
        SDL_Texture* _texture;
        inline const SDL_Rect& getBoundingBox() const { return _boundingBox; }
    
    public:
        TextureRectangle(SDL_Renderer* renderer, std::string filePath);
        TextureRectangle(SDL_Renderer* renderer, std::string filePath, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue);
        int getPositionX() override;
        int getPositionY() override;
        int getWidth() override;
        int getHeight() override;
        void setPosition(int x, int y) override;
        void setDimensions(int w, int h) override;
        void update() override;
        void render(SDL_Renderer* renderer) override;
        ~TextureRectangle();
};

#endif