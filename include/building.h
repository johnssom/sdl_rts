#ifndef BUILDING_H
#define BUILDING_H

#include <vector>
#include <string>
#include <functional>
#include <SDL2/SDL.h>

#define TILE_WIDTH  60
#define TILE_HEIGHT 30

#include "button_panel.h"

struct BuildingDiamond {
    float cx, cy, hw, hh;
    float topX, topY, rightX, rightY, bottomX, bottomY, leftX, leftY;
};

class Building {
    private:
        SDL_Renderer* _renderer;
        int _tileX, _tileY;
        int _tileW, _tileH;
        bool _isProducing;
        double _productionTimer;
        double _productionTime;
        std::vector<ButtonDefinition> _buttonDefs;

    public:
        Building(SDL_Renderer* renderer, int tileX, int tileY, int tileW, int tileH, double productionTime);

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

        void addButtonDef(std::string imagePath, int w, int h, std::function<void()> onClick);
        const std::vector<ButtonDefinition>& getButtonDefs() const;

        BuildingDiamond getBuildingDiamond() const;
        bool isInsideFootprint(int px, int py) const;

        static bool isInsideAnyFootprint(int px, int py, const std::vector<Building>& buildings);
};

#endif
