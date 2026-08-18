#include "game_entity.h"

GameEntity::GameEntity() {
    _sprite = nullptr;
}

GameEntity::GameEntity(SDL_Renderer* renderer) {
    _renderer = renderer;
    _sprite = nullptr;
}

GameEntity::GameEntity(SDL_Renderer* renderer, std::string filePath) {
    _renderer = renderer;
    _sprite = new TextureRectangle(_renderer, filePath);
}

TextureRectangle& GameEntity::getSprite() {
    return *_sprite;
}

Collider& GameEntity::getCollider(size_t index) {
    return *_colliders[index];
}

void GameEntity::addRectangleTextureComponent(std::string filePath) {
    _sprite = new TextureRectangle(_renderer, filePath);
}

void GameEntity::addRectangleTextureComponent(std::string filePath, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue) {
    _sprite = new TextureRectangle(_renderer, filePath, colorKeyRed, colorKeyGreen, colorKeyBlue);
}

void GameEntity::addCollider() {
    _colliders.push_back(new Collider());
}

void GameEntity::setPosition(int x, int y) {
    if (nullptr != _sprite) {
        _sprite->setPosition(x, y);
    }
    for (int i = 0; i < _colliders.size(); i++) {
        if (nullptr != _colliders[i]) {
            _colliders[i]->setPosition(x, y);
        }
    }
}

void GameEntity::setDimensions(int w, int h) {
    if (nullptr != _sprite) {
        _sprite->setDimensions(w, h);
    }
    for (int i = 0; i < _colliders.size(); i++) {
        if (nullptr != _colliders[i]) {
            _colliders[i]->setDimensions(w, h);
        }
    }
}

void GameEntity::update() {
    // if (nullptr != _sprite) {
    //     int x = _sprite->getPositionX();
    //     int y = _sprite->getPositionY();
    //     int w = _sprite->getWidth();
    //     int h = _sprite->getHeight();
    //     if (nullptr != _collider) {
    //         _collider->setPosition(x, y);
    //         _collider->setDimensions(w, h);
    //     }
    // }
}

void GameEntity::render() {
    if (nullptr != _sprite) {
        _sprite->render(_renderer);
    }
    for (int i = 0; i < _colliders.size(); i++) {
        if (nullptr != _colliders[i]) {
            SDL_SetRenderDrawColor(_renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(_renderer, &_colliders[i]->getHitBox());
        }
    }
}

GameEntity::~GameEntity() {
    if (nullptr != _sprite) {
        delete _sprite;
    }
    for (int i = 0; i < _colliders.size(); i++) {
        delete _colliders[i];
    }
}