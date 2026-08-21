#include "unit_entity.h"
#include <iostream>
#include <cmath>

UnitEntity::UnitEntity(GameEntity* gameEntity) {
    _gameEntity = gameEntity;
}

GameEntity& UnitEntity::getGameEntity() {
    return *_gameEntity;
}

void UnitEntity::setTeam(int team) {
    _team = team;
}

int UnitEntity::getTeam() const {
    return _team;
}

int UnitEntity::getHealth() const {
    return _health;
}

int UnitEntity::getMaxHealth() const {
    return _maxHealth;
}

void UnitEntity::setHealth(int health) {
    _health = health;
}

bool UnitEntity::isDead() const {
    return _health <= 0;
}

int UnitEntity::getFireCooldown() const {
    return _fireCooldown;
}

void UnitEntity::setFireCooldown(int cooldown) {
    _fireCooldown = cooldown;
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
        int currX = _gameEntity->getPositionX();
        int currY = _gameEntity->getPositionY();
        std::vector<std::pair<int, int>> path = _pathGrid->findPath(currX, currY, x, y);
        if (!path.empty()) {
            pushAction(std::make_unique<PathAction>(path, _allUnitsPtr, x, y, _pathGrid));
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

Action* UnitEntity::getCurrentAction() {
    if (_commandQueue.empty()) return nullptr;
    return _commandQueue.front().get();
}

void UnitEntity::update() {
    if (_commandQueue.empty()) {
        _isMoving = false;
        _animFrame = 1;
        _animTimer = 0;
        return;
    }

    int prevX = _gameEntity->getPositionX();
    int prevY = _gameEntity->getPositionY();

    if(_commandQueue.front()->updateUnit(*this)) {
        std::cout << "Finished at (" << _gameEntity->getPositionX() << ", " << _gameEntity->getPositionY() << ") completed action\n";
        _commandQueue.erase(_commandQueue.begin());
        _isMoving = false;
        _animFrame = 1;
        _animTimer = 0;
        return;
    }

    int deltaX = _gameEntity->getPositionX() - prevX;
    int deltaY = _gameEntity->getPositionY() - prevY;

    if (deltaX != 0 || deltaY != 0) {
        _isMoving = true;
        if (std::abs(deltaX) >= std::abs(deltaY)) {
            _direction = (deltaX > 0) ? DIR_DOWN_RIGHT : DIR_UP_LEFT;
        } else {
            _direction = (deltaY > 0) ? DIR_DOWN_LEFT : DIR_UP_RIGHT;
        }
        _animTimer++;
        if (_animTimer >= 3) {
            _animTimer = 0;
            _animFrame = (_animFrame + 1) % 3;
        }
    }
}

Direction UnitEntity::getDirection() const {
    return _direction;
}

void UnitEntity::setDirection(Direction direction) {
    _direction = direction;
}

int UnitEntity::getAnimFrame() const {
    return _animFrame;
}

bool UnitEntity::isMoving() const {
    return _isMoving;
}

void UnitEntity::incrementAnimFrame() {
    _animFrame = (_animFrame + 1) % 3;
}

UnitEntity::~UnitEntity() {
    delete _gameEntity;
}