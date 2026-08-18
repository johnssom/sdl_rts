#ifndef COLLIDER_H
#define COLLIDER

#include <SDL2/SDL.h>

class Collider {
    private:
        SDL_Rect* _hitBox;
    public:
        Collider();
        ~Collider();
        SDL_Rect& getHitBox();
        int getPositionX();
        int getPositionY();
        int getWidth();
        int getHeight();
        SDL_bool isColliding(Collider& Collider);
        void setPosition(int x, int y);
        void setDimensions(int w, int h);
};

#endif