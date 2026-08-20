#ifndef PATH_GRID_H
#define PATH_GRID_H

#include <vector>
#include <utility>

#define PATH_CELL_WIDTH  30
#define PATH_CELL_HEIGHT 15

class PathGrid {
    private:
        int _width;
        int _height;
        bool* _walkable;

    public:
        struct GridCoord { int x, y; };

    public:
        PathGrid(int mapSize);
        ~PathGrid();

        int getGridWidth() const;
        int getGridHeight() const;

        bool isWalkable(int gridX, int gridY) const;
        void setWalkable(int gridX, int gridY, bool walkable);
        void markTile(int gridX, int gridY);

        GridCoord pixelToGrid(int px, int py) const;
        std::pair<int, int> gridToPixel(int gx, int gy) const;

        static void toISO(int x, int y, int* sx, int* sy);
        static void tileToISO(int tx, int ty, int* sx, int* sy);
        static void fromISO(int px, int py, int* gx, int* gy);

        bool isInBounds(int gridX, int gridY) const;
        std::vector<std::pair<int, int>> findPath(int startX, int startY, int endX, int endY);
};

#endif
