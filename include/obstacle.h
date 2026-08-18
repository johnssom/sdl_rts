#ifndef OBSTACLE_H
#define OBSTACLE_H

struct Obstacle {
    int x, y, w, h;
    Obstacle(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
};

#endif
