#ifndef BUILDING_H
#define BUILDING_H

#include <vector>
#include <SDL2/SDL.h>

#define TILE_WIDTH  60
#define TILE_HEIGHT 30

struct BuildingDiamond {
    float cx, cy, hw, hh;
    float topX, topY, rightX, rightY, bottomX, bottomY, leftX, leftY;
};

class Building {
    private:
        int _tileX, _tileY;
        int _tileW, _tileH;
        bool _isProducing;
        double _productionTimer;
        double _productionTime;

    public:
        Building(int tileX, int tileY, int tileW, int tileH, double productionTime);

        int getTileX() const;
        int getTileY() const;
        int getTileW() const;
        int getTileH() const;
        bool isProducing() const;
        double getProgress() const;

        void startProduction();
        void update(double deltaTime);
        bool isDone() const;
        void resetProduction();

        BuildingDiamond getBuildingDiamond() const;
        bool isInsideFootprint(int px, int py) const;

        static bool isInsideAnyFootprint(int px, int py, const std::vector<Building>& buildings);
};

#endif
