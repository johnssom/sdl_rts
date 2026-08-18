#ifndef UNIT_ENTITY_H
#define UNIT_ENTITY_H

#include <vector>
#include <memory>

#include "game_entity.h"
#include "action.h"
#include "obstacle.h"

class UnitEntity {
    private:
        GameEntity* _gameEntity;
        std::vector<std::unique_ptr<Action>> _commandQueue;
        int _health = 80;
        bool _isSelected = false;
        std::vector<std::shared_ptr<UnitEntity>>* _allUnitsPtr = nullptr;
        std::vector<Obstacle>* _obstaclesPtr = nullptr;

    public:
        UnitEntity(GameEntity* gameEntity);
        GameEntity& getGameEntity();
        void pushAction(std::unique_ptr<Action> newAction);
        void commandMove(int x, int y);
        void queueMoveCommand(int x, int y);
        void setUnitsContext(std::vector<std::shared_ptr<UnitEntity>>* unitsPtr);
        void setObstaclesContext(std::vector<Obstacle>* obstaclesPtr);
        void update();
        ~UnitEntity();

};

#endif