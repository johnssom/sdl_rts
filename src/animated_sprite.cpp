#include "animated_sprite.h"

AnimatedSprite::AnimatedSprite(SDL_Renderer* renderer, std::string filePath) {
    SDL_Surface* surface = ResourceManager::getInstance().getSurface(filePath);
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0xFF, 0, 0xFF));
    _texture = SDL_CreateTextureFromSurface(renderer, surface);
}

void AnimatedSprite::setBoundingBox(int x, int y, int w, int h) {
    _boundingBox.x = x;
    _boundingBox.y = y;
    _boundingBox.w = w;
    _boundingBox.h = h;
}

void AnimatedSprite::playFrame(int x, int y, int w, int h, int frame) {
    _spriteSheet.x = x + w * (frame % 3);
    _spriteSheet.y = y; // + h * (frame % 4);
    _spriteSheet.w = w;
    _spriteSheet.h = h;
}

void AnimatedSprite::update() {

}

void AnimatedSprite::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, _texture, &_spriteSheet, &_boundingBox);
}

AnimatedSprite::~AnimatedSprite() {
    SDL_DestroyTexture(_texture);
}