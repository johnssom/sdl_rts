#include "dynamic_text.h"

bool DynamicText::_ttfInitialized = false;

DynamicText::DynamicText(std::string filePath, int pointSize) {
    if (!_ttfInitialized && TTF_Init() == -1) {
        std::cerr << "Failed to initialize TTF. ERROR: (" << TTF_GetError() << ")\n";
    } else {
        _ttfInitialized = true;
    }

    _font = TTF_OpenFont(filePath.c_str(), pointSize);
    if (_font == nullptr) {
        std::cerr << "Could not load font. ERROR: (" << TTF_GetError() << ")\n";
        exit(1);
    }
}

void DynamicText::render(SDL_Renderer* renderer, std::string text, int x, int y, int w, int h) {
    _surface = TTF_RenderText_Solid(_font, text.c_str(), { 255, 255, 255 });
    _texture = SDL_CreateTextureFromSurface(renderer, _surface);
    SDL_FreeSurface(_surface);
    _boundingBox.x = x;
    _boundingBox.y = y;
    _boundingBox.w = w;
    _boundingBox.h = h;
    SDL_RenderCopy(renderer, _texture, NULL, &_boundingBox);
    if(_texture != nullptr){
        SDL_DestroyTexture(_texture);
    }
}

DynamicText::~DynamicText() {
    if (_texture) {
        SDL_DestroyTexture(_texture);
    }
    if (_font) {
        TTF_CloseFont(_font);
    }
}