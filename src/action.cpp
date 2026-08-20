#include "action.h"
#include "unit_entity.h"
#include "path_grid.h"
#include <cmath>
#include <iostream>

MoveAction::MoveAction(int x, int y) {
    _targetX = x;
    _targetY = y;
    _unitsPtr = nullptr;
    _obstaclesPtr = nullptr;
}

MoveAction::MoveAction(int x, int y, std::vector<std::shared_ptr<UnitEntity>>* units) {
    _targetX = x;
    _targetY = y;
    _unitsPtr = units;
    _obstaclesPtr = nullptr;
}

MoveAction::MoveAction(int x, int y, std::vector<std::shared_ptr<UnitEntity>>* units, std::vector<Obstacle>* obstacles) {
    _targetX = x;
    _targetY = y;
    _unitsPtr = units;
    _obstaclesPtr = obstacles;
}

double MoveAction::getGroupCentroidX() const {
    if (!_unitsPtr || _unitsPtr->empty()) {
        return static_cast<double>(_targetX);
    }
    double totalX = 0.0;
    for (const auto& unit : *_unitsPtr) {
        totalX += unit->getGameEntity().getPositionX();
    }
    return totalX / static_cast<double>(_unitsPtr->size());
}

double MoveAction::getGroupCentroidY() const {
    if (!_unitsPtr || _unitsPtr->empty()) {
        return static_cast<double>(_targetY);
    }
    double totalY = 0.0;
    for (const auto& unit : *_unitsPtr) {
        totalY += unit->getGameEntity().getPositionY();
    }
    return totalY / static_cast<double>(_unitsPtr->size());
}

bool MoveAction::isGroupAtTarget() const {
    double centroidX = getGroupCentroidX();
    double centroidY = getGroupCentroidY();
    double deltaX = _targetX - centroidX;
    double deltaY = _targetY - centroidY;
    return std::sqrt(deltaX * deltaX + deltaY * deltaY) == 0.0;
}

void MoveAction::calculateSeparation(UnitEntity& unit, double& outX, double& outY) {
    outX = 0.0;
    outY = 0.0;
    if (!_unitsPtr || _unitsPtr->empty()) return;

    int currX = unit.getGameEntity().getPositionX();
    int currY = unit.getGameEntity().getPositionY();
    int count = 0;

    for (const auto& other : *_unitsPtr) {
        if (other.get() == &unit) continue;

        int otherX = other->getGameEntity().getPositionX();
        int otherY = other->getGameEntity().getPositionY();

        double diffX = currX - otherX;
        double diffY = currY - otherY;
        double distance = std::sqrt(diffX * diffX + diffY * diffY);

        if (distance < SEPARATION_RADIUS && distance > 0.1) {
            outX += diffX / distance;
            outY += diffY / distance;
            count++;
        }
    }

    if (count > 0) {
        outX /= count;
        outY /= count;
    }
}

void MoveAction::calculateAlignment(UnitEntity& unit, double& outX, double& outY) {
    outX = 0.0;
    outY = 0.0;
    if (!_unitsPtr || _unitsPtr->empty()) return;

    int currX = unit.getGameEntity().getPositionX();
    int currY = unit.getGameEntity().getPositionY();
    int count = 0;

    for (const auto& other : *_unitsPtr) {
        if (other.get() == &unit) continue;

        int otherX = other->getGameEntity().getPositionX();
        int otherY = other->getGameEntity().getPositionY();

        double diffX = otherX - currX;
        double diffY = otherY - currY;
        double distance = std::sqrt(diffX * diffX + diffY * diffY);

        if (distance < BOID_RADIUS && distance > 0.1) {
            double targetDX = _targetX - otherX;
            double targetDY = _targetY - otherY;
            double targetDist = std::sqrt(targetDX * targetDX + targetDY * targetDY);
            if (targetDist > 0.1) {
                outX += targetDX / targetDist;
                outY += targetDY / targetDist;
            }
            count++;
        }
    }

    if (count > 0) {
        outX /= count;
        outY /= count;
    }
}

void MoveAction::calculateCohesion(UnitEntity& unit, double& outX, double& outY) {
    outX = 0.0;
    outY = 0.0;
    if (!_unitsPtr || _unitsPtr->empty()) return;

    int currX = unit.getGameEntity().getPositionX();
    int currY = unit.getGameEntity().getPositionY();
    double avgX = 0.0;
    double avgY = 0.0;
    int count = 0;

    for (const auto& other : *_unitsPtr) {
        if (other.get() == &unit) continue;

        int otherX = other->getGameEntity().getPositionX();
        int otherY = other->getGameEntity().getPositionY();

        double diffX = otherX - currX;
        double diffY = otherY - currY;
        double distance = std::sqrt(diffX * diffX + diffY * diffY);

        if (distance < BOID_RADIUS) {
            avgX += otherX;
            avgY += otherY;
            count++;
        }
    }

    if (count > 0) {
        avgX /= count;
        avgY /= count;
        double dirX = avgX - currX;
        double dirY = avgY - currY;
        double dist = std::sqrt(dirX * dirX + dirY * dirY);
        if (dist > 0.1) {
            outX = dirX / dist;
            outY = dirY / dist;
        }
    }
}

void MoveAction::calculateObstacleAvoidance(UnitEntity& unit, double& outX, double& outY) {
    outX = 0.0;
    outY = 0.0;
    if (!_obstaclesPtr || _obstaclesPtr->empty()) return;

    double unitCX = unit.getGameEntity().getPositionX();
    double unitCY = unit.getGameEntity().getPositionY();

    for (const auto& obs : *_obstaclesPtr) {
        int obsSX, obsSY;
        PathGrid::tileToISO(obs.x, obs.y, &obsSX, &obsSY);
        double obsCX = obsSX + 30.0;
        double obsCY = obsSY + 15.0;

        double diffX = unitCX - obsCX;
        double diffY = unitCY - obsCY;
        double distance = std::sqrt(diffX * diffX + diffY * diffY);

        double avoidRadius = OBSTACLE_AVOIDANCE_RADIUS;

        if (distance < avoidRadius && distance > 0.1) {
            double force = 1.0 - (distance / avoidRadius);
            outX += (diffX / distance) * force;
            outY += (diffY / distance) * force;
        }
    }
}

bool MoveAction::updateUnit(UnitEntity& unit) {
    if (isGroupAtTarget()) {
        return true;
    }

    int currX = unit.getGameEntity().getPositionX();
    int currY = unit.getGameEntity().getPositionY();

    double targetDeltaX = _targetX - currX;
    double targetDeltaY = _targetY - currY;
    double targetDistance = std::sqrt(targetDeltaX * targetDeltaX + targetDeltaY * targetDeltaY);

    if (targetDistance < 1) return false;

    targetDeltaX /= targetDistance;
    targetDeltaY /= targetDistance;

    double sepX = 0.0, sepY = 0.0;
    calculateSeparation(unit, sepX, sepY);

    double aliX = 0.0, aliY = 0.0;
    calculateAlignment(unit, aliX, aliY);

    double cohX = 0.0, cohY = 0.0;
    calculateCohesion(unit, cohX, cohY);

    double obsX = 0.0, obsY = 0.0;
    calculateObstacleAvoidance(unit, obsX, obsY);

    double finalDeltaX = targetDeltaX * TARGET_WEIGHT
                       + sepX * SEPARATION_WEIGHT
                       + aliX * ALIGNMENT_WEIGHT
                       + cohX * COHESION_WEIGHT
                       + obsX * OBSTACLE_AVOIDANCE_WEIGHT;
    double finalDeltaY = targetDeltaY * TARGET_WEIGHT
                       + sepY * SEPARATION_WEIGHT
                       + aliY * ALIGNMENT_WEIGHT
                       + cohY * COHESION_WEIGHT
                       + obsY * OBSTACLE_AVOIDANCE_WEIGHT;

    double finalDist = std::sqrt(finalDeltaX * finalDeltaX + finalDeltaY * finalDeltaY);
    if (finalDist > MAX_SPEED) {
        finalDeltaX = (finalDeltaX / finalDist) * MAX_SPEED;
        finalDeltaY = (finalDeltaY / finalDist) * MAX_SPEED;
    }

    unit.getGameEntity().setPosition(currX + std::round(finalDeltaX), currY + std::round(finalDeltaY));
    return false;
}

PathAction::PathAction(std::vector<std::pair<int, int>> path, std::vector<std::shared_ptr<UnitEntity>>* units, int destX, int destY, PathGrid* pathGrid) {
    _path = path;
    _currentWaypoint = 0;
    _unitsPtr = units;
    _destX = destX;
    _destY = destY;
    _pathGrid = pathGrid;
    _recalcTimer = 0;
    _recalcInterval = 30;
}

void PathAction::calculateSeparation(UnitEntity& unit, double& outX, double& outY) {
    outX = 0.0;
    outY = 0.0;
    if (!_unitsPtr || _unitsPtr->empty()) return;

    int currX = unit.getGameEntity().getPositionX();
    int currY = unit.getGameEntity().getPositionY();
    int count = 0;

    for (const auto& other : *_unitsPtr) {
        if (other.get() == &unit) continue;

        int otherX = other->getGameEntity().getPositionX();
        int otherY = other->getGameEntity().getPositionY();

        double diffX = currX - otherX;
        double diffY = currY - otherY;
        double distance = std::sqrt(diffX * diffX + diffY * diffY);

        if (distance < SEPARATION_RADIUS && distance > 0.1) {
            outX += diffX / distance;
            outY += diffY / distance;
            count++;
        }
    }

    if (count > 0) {
        outX /= count;
        outY /= count;
    }
}

bool PathAction::updateUnit(UnitEntity& unit) {
    if (_path.empty()) return true;

    int currX = unit.getGameEntity().getPositionX();
    int currY = unit.getGameEntity().getPositionY();

    if (_pathGrid) {
        int gx, gy;
        PathGrid::fromISO(currX, currY, &gx, &gy);
        if (!_pathGrid->isWalkable(gx, gy)) {
            std::vector<std::pair<int, int>> newPath = _pathGrid->findPath(currX, currY, _destX, _destY);
            if (!newPath.empty()) {
                _path = newPath;
                _currentWaypoint = 0;
                currX = unit.getGameEntity().getPositionX();
                currY = unit.getGameEntity().getPositionY();
            }
        }
    }

    _recalcTimer++;
    if (_recalcTimer >= _recalcInterval && _pathGrid) {
        _recalcTimer = 0;
        std::vector<std::pair<int, int>> newPath = _pathGrid->findPath(currX, currY, _destX, _destY);
        if (!newPath.empty()) {
            _path = newPath;
            _currentWaypoint = 0;
        }
    }

    auto& wp = _path[_currentWaypoint];
    double wpDeltaX = wp.first - currX;
    double wpDeltaY = wp.second - currY;
    double wpDist = std::sqrt(wpDeltaX * wpDeltaX + wpDeltaY * wpDeltaY);
    if (wpDist < WAYPOINT_REACH_DIST) {
        _currentWaypoint++;
        if (_currentWaypoint >= (int)_path.size()) {
            return true;
        }
        return false;
    }

    wpDeltaX /= wpDist;
    wpDeltaY /= wpDist;

    double sepX = 0.0, sepY = 0.0;
    calculateSeparation(unit, sepX, sepY);

    double finalDeltaX = wpDeltaX * WAYPOINT_WEIGHT + sepX * SEPARATION_WEIGHT;
    double finalDeltaY = wpDeltaY * WAYPOINT_WEIGHT + sepY * SEPARATION_WEIGHT;

    double finalDist = std::sqrt(finalDeltaX * finalDeltaX + finalDeltaY * finalDeltaY);
    if (finalDist > MAX_SPEED) {
        finalDeltaX = (finalDeltaX / finalDist) * MAX_SPEED;
        finalDeltaY = (finalDeltaY / finalDist) * MAX_SPEED;
    }
    int moveX = finalDeltaX > 0 ? (int)std::ceil(finalDeltaX) : (int)std::floor(finalDeltaX);
    int moveY = finalDeltaY > 0 ? (int)std::ceil(finalDeltaY) : (int)std::floor(finalDeltaY);
    int newX = currX + moveX;
    int newY = currY + moveY;

    if (_pathGrid) {
        int ngx, ngy;
        PathGrid::fromISO(newX, newY, &ngx, &ngy);
        if (!_pathGrid->isWalkable(ngx, ngy)) {
            return false;
        }
    }

    unit.getGameEntity().setPosition(newX, newY);
    return false;
}
