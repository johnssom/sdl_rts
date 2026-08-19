#include "path_grid.h"
#include <cmath>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <functional>

static const int TILE_WIDTH = 60;
static const int TILE_HEIGHT = 30;
static const int MAP_RENDER_SIZE = 24;
static const int SCREEN_WIDTH = 1600;
static const int SCREEN_HEIGHT = 900;
static const int MAP_RENDER_OFFSET_X = (SCREEN_WIDTH - (TILE_WIDTH * MAP_RENDER_SIZE)) / 2;
static const int MAP_RENDER_OFFSET_Y = 425;

struct PairHash {
    size_t operator()(const std::pair<int,int>& p) const {
        size_t h1 = std::hash<int>()(p.first);
        size_t h2 = std::hash<int>()(p.second);
        return h1 ^ (h2 * 2654435761u);
    }
};

PathGrid::PathGrid(int mapSize) {
    _width = mapSize;
    _height = mapSize;
    _walkable = new bool[_width * _height];
    for (int i = 0; i < _width * _height; i++) {
        _walkable[i] = true;
    }
}

PathGrid::~PathGrid() {
    delete[] _walkable;
}

int PathGrid::getGridWidth() const { return _width; }
int PathGrid::getGridHeight() const { return _height; }

bool PathGrid::isWalkable(int gridX, int gridY) const {
    if (!isInBounds(gridX, gridY)) return false;
    return _walkable[gridY * _width + gridX];
}

void PathGrid::setWalkable(int gridX, int gridY, bool walkable) {
    if (isInBounds(gridX, gridY)) {
        _walkable[gridY * _width + gridX] = walkable;
    }
}

void PathGrid::markTile(int gridX, int gridY) {
    setWalkable(gridX, gridY, false);
}

void PathGrid::toISO(int x, int y, int* sx, int* sy) {
    *sx = MAP_RENDER_OFFSET_X + (x * TILE_WIDTH / 2 + y * TILE_WIDTH / 2);
    *sy = MAP_RENDER_OFFSET_Y + (y * TILE_HEIGHT / 2 - x * TILE_HEIGHT / 2);
}

void PathGrid::fromISO(int px, int py, int* gx, int* gy) {
    int dx = px - MAP_RENDER_OFFSET_X - TILE_WIDTH / 2;
    int dy = py - MAP_RENDER_OFFSET_Y - TILE_HEIGHT / 2;
    double tx = (dx / (double)TILE_WIDTH - dy / (double)TILE_HEIGHT);
    double ty = (dx / (double)TILE_WIDTH + dy / (double)TILE_HEIGHT);
    *gx = (int)std::round(tx);
    *gy = (int)std::round(ty);
}

PathGrid::GridCoord PathGrid::pixelToGrid(int px, int py) const {
    int gx, gy;
    fromISO(px, py, &gx, &gy);
    gx = std::max(0, std::min(gx, _width - 1));
    gy = std::max(0, std::min(gy, _height - 1));
    return {gx, gy};
}

std::pair<int, int> PathGrid::gridToPixel(int gx, int gy) const {
    int sx, sy;
    toISO(gx, gy, &sx, &sy);
    return {sx + TILE_WIDTH / 2, sy + TILE_HEIGHT / 2};
}

bool PathGrid::isInBounds(int gridX, int gridY) const {
    return gridX >= 0 && gridX < _width && gridY >= 0 && gridY < _height;
}

std::vector<std::pair<int, int>> PathGrid::findPath(int startX, int startY, int endX, int endY) {
    std::vector<std::pair<int, int>> path;

    GridCoord start = pixelToGrid(startX, startY);
    GridCoord end = pixelToGrid(endX, endY);

    if (!isWalkable(start.x, start.y) || !isWalkable(end.x, end.y)) {
        return path;
    }

    if (start.x == end.x && start.y == end.y) {
        auto p = gridToPixel(end.x, end.y);
        path.push_back(p);
        return path;
    }

    struct PathNode {
        int x, y;
        float g, f;
        int parentIdx;
    };

    struct Compare {
        bool operator()(const PathNode& a, const PathNode& b) const { return a.f > b.f; }
    };

    std::priority_queue<PathNode, std::vector<PathNode>, Compare> open;
    std::vector<PathNode> closed;
    std::unordered_set<std::pair<int,int>, PairHash> closedSet;
    std::unordered_map<std::pair<int,int>, int, PairHash> closedIndex;

    float startH = std::sqrt(float((end.x - start.x) * (end.x - start.x) + (end.y - start.y) * (end.y - start.y)));
    open.push({start.x, start.y, 0.0f, startH, -1});

    static const int dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
    static const int dy[] = {1, -1, 0, 0, 1, -1, 1, -1};
    static const float costs[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.414f, 1.414f, 1.414f, 1.414f};

    while (!open.empty()) {
        PathNode current = open.top();
        open.pop();

        std::pair<int,int> currentKey = {current.x, current.y};

        if (closedSet.count(currentKey)) continue;

        int currentIdx = (int)closed.size();
        closed.push_back(current);
        closedSet.insert(currentKey);
        closedIndex[currentKey] = currentIdx;

        if (current.x == end.x && current.y == end.y) {
            int idx = currentIdx;
            while (idx != -1) {
                path.push_back(gridToPixel(closed[idx].x, closed[idx].y));
                idx = closed[idx].parentIdx;
            }
            std::reverse(path.begin(), path.end());
            if (path.size() > 1) {
                path.erase(path.begin());
            }
            return path;
        }

        for (int i = 0; i < 8; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];
            std::pair<int,int> neighborKey = {nx, ny};

            if (!isWalkable(nx, ny) || closedSet.count(neighborKey)) continue;

            if (i >= 4) {
                if (!isWalkable(current.x + dx[i], current.y) || !isWalkable(current.x, current.y + dy[i])) continue;
            }

            float newG = current.g + costs[i];
            float h = std::sqrt(float((end.x - nx) * (end.x - nx) + (end.y - ny) * (end.y - ny)));
            open.push({nx, ny, newG, newG + h, currentIdx});
        }
    }

    return path;
}
