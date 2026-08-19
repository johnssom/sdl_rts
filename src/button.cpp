#include "button.h"
#include "texture_rectangle.h"

Button::Button(SDL_Renderer* renderer, std::string filePath, int x, int y, int w, int h)
    : _x(x), _y(y), _w(w), _h(h) {
    _sprite = new TextureRectangle(renderer, filePath);
    _sprite->setPosition(x, y);
    _sprite->setDimensions(w, h);
}

int Button::getX() const { return _x; }
int Button::getY() const { return _y; }
int Button::getWidth() const { return _w; }
int Button::getHeight() const { return _h; }

void Button::setPosition(int x, int y) {
    _x = x;
    _y = y;
    _sprite->setPosition(x, y);
}

void Button::setDimensions(int w, int h) {
    _w = w;
    _h = h;
    _sprite->setDimensions(w, h);
}

void Button::setOnClick(std::function<void()> callback) {
    _onClick = callback;
}

bool Button::containsPoint(int px, int py) const {
    return px >= _x && px <= _x + _w && py >= _y && py <= _y + _h;
}

void Button::handleClick() {
    if (_onClick) {
        _onClick();
    }
}

void Button::render(SDL_Renderer* renderer) {
    _sprite->render(renderer);
}

Button::~Button() {
    delete _sprite;
}
