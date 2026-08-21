#ifndef UNIT_ENTITY_H
#define UNIT_ENTITY_H

#include <vector>
#include <memory>

#include "game_entity.h"
#include "action.h"
#include "obstacle.h"
#include "path_grid.h"

enum Direction {
    DIR_DOWN_LEFT = 0,
    DIR_UP_LEFT = 1,
    DIR_DOWN_RIGHT = 2,
    DIR_UP_RIGHT = 3
};

class UnitEntity {
    private:
        GameEntity* _gameEntity;
        std::vector<std::unique_ptr<Action>> _commandQueue;
        int _health = 80;
        int _maxHealth = 80;
        bool _isSelected = false;
        std::vector<std::shared_ptr<UnitEntity>>* _allUnitsPtr = nullptr;
        std::vector<Obstacle>* _obstaclesPtr = nullptr;
        PathGrid* _pathGrid = nullptr;
        Direction _direction = DIR_DOWN_LEFT;
        int _animFrame = 0;
        bool _isMoving = false;
        int _animTimer = 0;
        int _team = 0;
        int _fireCooldown = 0;

    public:
        UnitEntity(GameEntity* gameEntity);
        GameEntity& getGameEntity();
        void setTeam(int team);
        int getTeam() const;
        int getHealth() const;
        int getMaxHealth() const;
        void setHealth(int health);
        bool isDead() const;
        int getFireCooldown() const;
        void setFireCooldown(int cooldown);
        void pushAction(std::unique_ptr<Action> newAction);
        void commandMove(int x, int y);
        void queueMoveCommand(int x, int y);
        void setUnitsContext(std::vector<std::shared_ptr<UnitEntity>>* unitsPtr);
        void setObstaclesContext(std::vector<Obstacle>* obstaclesPtr);
        void setPathGrid(PathGrid* pathGrid);
        Action* getCurrentAction();
        Direction getDirection() const;
        void setDirection(Direction direction);
        int getAnimFrame() const;
        bool isMoving() const;
        void incrementAnimFrame();
        void update();
        ~UnitEntity();

};

#endif