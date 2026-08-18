#include "collider.h"

Collider::Collider() {
    _hitBox = new SDL_Rect;
    setPosition(0, 0);
    setDimensions(0, 0);
}

Collider::~Collider() {
    
}

SDL_Rect& Collider::getHitBox() {
    return *_hitBox;
}

int Collider::getPositionX() {
    return _hitBox->x;
}

int Collider::getPositionY() {
    return _hitBox->y;
}

int Collider::getHeight() {
    return _hitBox->h;
}

int Collider::getWidth() {
    return _hitBox->w;
}

SDL_bool Collider::isColliding(Collider& collider) {
    SDL_Rect targetHitBox = collider.getHitBox();
    if (nullptr == _hitBox) {
        return SDL_FALSE;
    }
    return SDL_HasIntersection(_hitBox, &targetHitBox);
}

void Collider::setPosition(int x, int y) {
    _hitBox->x = x;
    _hitBox->y = y;
}

void Collider::setDimensions(int w, int h) {
    _hitBox->w = w;
    _hitBox->h = h;
}