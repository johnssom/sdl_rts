#include "unit_entity.h"
#include <iostream>

UnitEntity::UnitEntity(GameEntity* gameEntity) {
    _gameEntity = gameEntity;
}

GameEntity& UnitEntity::getGameEntity() {
    return *_gameEntity;
}

void UnitEntity::commandMove(int x, int y) {
    _commandQueue.clear();
    queueMoveCommand(x, y);
}

void UnitEntity::pushAction(std::unique_ptr<Action> newAction) {
    _commandQueue.push_back(std::move(newAction));
}

void UnitEntity::queueMoveCommand(int x, int y) {
    if (_pathGrid) {
        int currX = _gameEntity->getSprite().getPositionX();
        int currY = _gameEntity->getSprite().getPositionY();
        std::vector<std::pair<int, int>> path = _pathGrid->findPath(currX, currY, x, y);
        if (!path.empty()) {
            pushAction(std::make_unique<PathAction>(path, _allUnitsPtr));
        }
    } else if (_allUnitsPtr && _obstaclesPtr) {
        pushAction(std::make_unique<MoveAction>(x, y, _allUnitsPtr, _obstaclesPtr));
    } else if (_allUnitsPtr) {
        pushAction(std::make_unique<MoveAction>(x, y, _allUnitsPtr));
    } else {
        pushAction(std::make_unique<MoveAction>(x, y));
    }
}

void UnitEntity::setUnitsContext(std::vector<std::shared_ptr<UnitEntity>>* unitsPtr) {
    _allUnitsPtr = unitsPtr;
}

void UnitEntity::setObstaclesContext(std::vector<Obstacle>* obstaclesPtr) {
    _obstaclesPtr = obstaclesPtr;
}

void UnitEntity::setPathGrid(PathGrid* pathGrid) {
    _pathGrid = pathGrid;
}

void UnitEntity::update() {
    if (_commandQueue.empty()) return;
    if(_commandQueue.front()->updateUnit(*this)) {
        std::cout << "Finished at (" << _gameEntity->getSprite().getPositionX() << ", " << _gameEntity->getSprite().getPositionY() << ") completed action\n";
        _commandQueue.erase(_commandQueue.begin());
    }
}

UnitEntity::~UnitEntity() {
    delete _gameEntity;
}