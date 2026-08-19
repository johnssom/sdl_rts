#include "loading_bar.h"

LoadingBar::LoadingBar(float x, float y, float w, float h)
    : _x(x), _y(y), _w(w), _h(h),
      _bgColor({40, 40, 40, SDL_ALPHA_OPAQUE}),
      _fillColor({0, 200, 80, SDL_ALPHA_OPAQUE}) {}

void LoadingBar::setPosition(float x, float y) {
    _x = x;
    _y = y;
}

void LoadingBar::setColors(SDL_Color bg, SDL_Color fill) {
    _bgColor = bg;
    _fillColor = fill;
}

void LoadingBar::render(SDL_Renderer* renderer, float progress) const {
    if (progress > 1.0f) progress = 1.0f;
    if (progress < 0.0f) progress = 0.0f;

    SDL_Rect bgRect = {(int)_x, (int)_y, (int)_w, (int)_h};
    SDL_SetRenderDrawColor(renderer, _bgColor.r, _bgColor.g, _bgColor.b, _bgColor.a);
    SDL_RenderFillRect(renderer, &bgRect);

    SDL_Rect fillRect = {(int)_x, (int)_y, (int)(_w * progress), (int)_h};
    SDL_SetRenderDrawColor(renderer, _fillColor.r, _fillColor.g, _fillColor.b, _fillColor.a);
    SDL_RenderFillRect(renderer, &fillRect);
}
