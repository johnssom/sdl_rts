#include "texture_rectangle.h" 

TextureRectangle::TextureRectangle(SDL_Renderer* renderer, std::string filePath) {
    SDL_Surface* surface = ResourceManager::getInstance().getSurface(filePath);
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0xFF, 0, 0xFF));
    _texture = SDL_CreateTextureFromSurface(renderer, surface);
    setPosition(0, 0);
    setDimensions(32, 32);
}

TextureRectangle::TextureRectangle(SDL_Renderer* renderer, std::string filePath, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue) {
    SDL_Surface* surface = ResourceManager::getInstance().getSurface(filePath);
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, colorKeyRed, colorKeyGreen, colorKeyBlue));
    _texture = SDL_CreateTextureFromSurface(renderer, surface);
    setPosition(0, 0);
    setDimensions(32, 32);
}

int TextureRectangle::getPositionX() {
    return _boundingBox.x;
}

int TextureRectangle::getPositionY() {
    return _boundingBox.y;
}

int TextureRectangle::getHeight() {
    return _boundingBox.h;
}

int TextureRectangle::getWidth() {
    return _boundingBox.w;
}

void TextureRectangle::setPosition(int x, int y) {
    _boundingBox.x = x;
    _boundingBox.y = y;
}

void TextureRectangle::setDimensions(int w, int h) {
    _boundingBox.w = w;
    _boundingBox.h = h;
}

void TextureRectangle::update() {

}

void TextureRectangle::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, _texture, NULL, &_boundingBox);
}

TextureRectangle::~TextureRectangle() {
    SDL_DestroyTexture(_texture);
}