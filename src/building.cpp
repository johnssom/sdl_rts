#include "building.h"
#include "path_grid.h"
#include <cmath>

Building::Building(int tileX, int tileY, int tileW, int tileH, double productionTime)
    : _tileX(tileX), _tileY(tileY), _tileW(tileW), _tileH(tileH),
      _isProducing(false), _productionTimer(0.0), _productionTime(productionTime) {}

int Building::getTileX() const { return _tileX; }
int Building::getTileY() const { return _tileY; }
int Building::getTileW() const { return _tileW; }
int Building::getTileH() const { return _tileH; }
bool Building::isProducing() const { return _isProducing; }

double Building::getProgress() const {
    if (_productionTime <= 0.0) return 0.0;
    return _productionTimer / _productionTime;
}

void Building::startProduction() {
    _isProducing = true;
    _productionTimer = 0.0;
}

void Building::update(double deltaTime) {
    if (_isProducing) {
        _productionTimer += deltaTime;
    }
}

bool Building::isDone() const {
    return _isProducing && _productionTimer >= _productionTime;
}

void Building::resetProduction() {
    _isProducing = false;
    _productionTimer = 0.0;
}

BuildingDiamond Building::getBuildingDiamond() const {
    int tsx, tsy, rsx, rsy, bsx, bsy, lsx, lsy;

    PathGrid::toISO(_tileX + _tileW - 1, _tileY, &tsx, &tsy);
    float topX = tsx + TILE_WIDTH / 2.0f;
    float topY = tsy;

    PathGrid::toISO(_tileX + _tileW - 1, _tileY + _tileH - 1, &rsx, &rsy);
    float rightX = rsx + TILE_WIDTH;
    float rightY = rsy + TILE_HEIGHT / 2.0f;

    PathGrid::toISO(_tileX, _tileY + _tileH - 1, &bsx, &bsy);
    float bottomX = bsx + TILE_WIDTH / 2.0f;
    float bottomY = bsy + TILE_HEIGHT;

    PathGrid::toISO(_tileX, _tileY, &lsx, &lsy);
    float leftX = lsx;
    float leftY = lsy + TILE_HEIGHT / 2.0f;

    float cx = (topX + bottomX) / 2.0f;
    float cy = (topY + bottomY) / 2.0f;
    float hw = (rightX - leftX) / 2.0f;
    float hh = (bottomY - topY) / 2.0f;

    return {cx, cy, hw, hh, topX, topY, rightX, rightY, bottomX, bottomY, leftX, leftY};
}

bool Building::isInsideFootprint(int px, int py) const {
    BuildingDiamond d = getBuildingDiamond();
    double dx = std::abs(px - d.cx) / d.hw;
    double dy = std::abs(py - d.cy) / d.hh;
    return (dx + dy <= 1.0);
}

bool Building::isInsideAnyFootprint(int px, int py, const std::vector<Building>& buildings) {
    for (const auto& building : buildings) {
        if (building.isInsideFootprint(px, py)) return true;
    }
    return false;
}
