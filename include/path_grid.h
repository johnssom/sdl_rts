#ifndef PATH_GRID_H
#define PATH_GRID_H

#include <vector>
#include <utility>
#include "obstacle.h"

class PathGrid {
    private:
        int _width;
        int _height;
        int _cellSize;
        bool* _walkable;

        struct GridCoord { int x, y; };

    public:
        PathGrid(int worldWidth, int worldHeight, int cellSize);
        ~PathGrid();

        int getCellSize() const;
        int getGridWidth() const;
        int getGridHeight() const;

        bool isWalkable(int gridX, int gridY) const;
        void setWalkable(int gridX, int gridY, bool walkable);
        void markObstacle(const Obstacle& obs);

        GridCoord pixelToGrid(int px, int py) const;
        std::pair<int, int> gridToPixel(int gx, int gy) const;

        bool isInBounds(int gridX, int gridY) const;
        std::vector<std::pair<int, int>> findPath(int startX, int startY, int endX, int endY);
};

#endif
