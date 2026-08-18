#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include <string>
#include <vector>
#include <SDL2/SDL.h>

#include "sprite.h"
#include "collider.h"

class GameEntity {
    private:
        SDL_Renderer* _renderer;
        Sprite* _sprite;
        std::vector<Collider*> _colliders;

    public:
        GameEntity();
        GameEntity(SDL_Renderer* renderer);
        GameEntity(SDL_Renderer* renderer, std::string filePath);
        Sprite& getSprite();
        Collider& getCollider(size_t index);
        void setSprite(Sprite* sprite);
        void addRectangleTextureComponent(std::string filePath);
        void addRectangleTextureComponent(std::string filePath, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue);
        void addCollider();
        void setPosition(int x, int y);
        void setDimensions(int w, int h);
        void update();
        void render();
        ~GameEntity();
};

#endif