#include "action.h"
#include "unit_entity.h"
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
        totalX += unit->getGameEntity().getSprite().getPositionX();
    }
    return totalX / static_cast<double>(_unitsPtr->size());
}

double MoveAction::getGroupCentroidY() const {
    if (!_unitsPtr || _unitsPtr->empty()) {
        return static_cast<double>(_targetY);
    }

    double totalY = 0.0;
    for (const auto& unit : *_unitsPtr) {
        totalY += unit->getGameEntity().getSprite().getPositionY();
    }
    return totalY / static_cast<double>(_unitsPtr->size());
}

bool MoveAction::isGroupAtTarget() const {
    double centroidX = getGroupCentroidX();
    double centroidY = getGroupCentroidY();
    double deltaX = _targetX - centroidX;
    double deltaY = _targetY - centroidY;
    std::cout << "Centroid: (" << centroidX << ", " << centroidY << "), Target: (" << _targetX << ", " << _targetY << "), Delta: (" << deltaX << ", " << deltaY << ")\n";
    return std::sqrt(deltaX * deltaX + deltaY * deltaY) < 1.0;
}

void MoveAction::calculateSeparation(UnitEntity& unit, double& outDeltaX, double& outDeltaY) {
    outDeltaX = 0.0;
    outDeltaY = 0.0;
    
    if (!_unitsPtr || _unitsPtr->empty()) return;
    
    int currX = unit.getGameEntity().getSprite().getPositionX();
    int currY = unit.getGameEntity().getSprite().getPositionY();
    
    for (const auto& otherUnit : *_unitsPtr) {
        // Skip self
        if (otherUnit.get() == &unit) continue;
        
        int otherX = otherUnit->getGameEntity().getSprite().getPositionX();
        int otherY = otherUnit->getGameEntity().getSprite().getPositionY();
        
        double diffX = currX - otherX;
        double diffY = currY - otherY;
        double distance = std::sqrt(diffX * diffX + diffY * diffY);
        
        // Only apply separation if within radius and closer than min distance
        if (distance < SEPARATION_RADIUS && distance > 0.1) {
            if (distance < MIN_SEPARATION_DISTANCE) {
                // Strong repulsion when too close
                double separationForce = 1.0 - (distance / SEPARATION_RADIUS);
                outDeltaX += (diffX / distance) * separationForce;
                outDeltaY += (diffY / distance) * separationForce;
            }
        }
    }
}

void MoveAction::calculateObstacleAvoidance(UnitEntity& unit, double& outDeltaX, double& outDeltaY) {
    outDeltaX = 0.0;
    outDeltaY = 0.0;

    if (!_obstaclesPtr || _obstaclesPtr->empty()) return;

    double unitCX = unit.getGameEntity().getSprite().getPositionX() + unit.getGameEntity().getSprite().getWidth() / 2.0;
    double unitCY = unit.getGameEntity().getSprite().getPositionY() + unit.getGameEntity().getSprite().getHeight() / 2.0;

    for (const auto& obs : *_obstaclesPtr) {
        double obsCX = obs.x + obs.w / 2.0;
        double obsCY = obs.y + obs.h / 2.0;

        double diffX = unitCX - obsCX;
        double diffY = unitCY - obsCY;
        double distance = std::sqrt(diffX * diffX + diffY * diffY);

        double avoidRadius = OBSTACLE_AVOIDANCE_RADIUS + (obs.w + obs.h) / 4.0;

        if (distance < avoidRadius && distance > 0.1) {
            double force = 1.0 - (distance / avoidRadius);
            outDeltaX += (diffX / distance) * force;
            outDeltaY += (diffY / distance) * force;
        }
    }
}

bool MoveAction::updateUnit(UnitEntity& unit) {
    if (isGroupAtTarget()) {
        return true;
    }

    int currX = unit.getGameEntity().getSprite().getPositionX();
    int currY = unit.getGameEntity().getSprite().getPositionY();
    
    double targetDeltaX = _targetX - currX;
    double targetDeltaY = _targetY - currY;
    double targetDistance = std::sqrt(targetDeltaX * targetDeltaX + targetDeltaY * targetDeltaY);
    
    if (targetDistance < 1) return false;
    
    // Normalize target direction
    targetDeltaX /= targetDistance;
    targetDeltaY /= targetDistance;
    
    // Calculate separation forces
    double separationDeltaX = 0.0;
    double separationDeltaY = 0.0;
    calculateSeparation(unit, separationDeltaX, separationDeltaY);

    // Calculate obstacle avoidance forces
    double obstacleDeltaX = 0.0;
    double obstacleDeltaY = 0.0;
    calculateObstacleAvoidance(unit, obstacleDeltaX, obstacleDeltaY);

    // Combine separation and obstacle avoidance
    double avoidanceDeltaX = separationDeltaX + obstacleDeltaX;
    double avoidanceDeltaY = separationDeltaY + obstacleDeltaY;

    // Keep the move order moving toward the destination; only use avoidance
    // as a corrective nudge when it is aligned with the goal.
    double avoidanceDotTarget = avoidanceDeltaX * targetDeltaX + avoidanceDeltaY * targetDeltaY;
    if (avoidanceDotTarget < 0.0) {
        avoidanceDeltaX = 0.0;
        avoidanceDeltaY = 0.0;
    }
    
    // Combine target movement and avoidance (weighted)
    double finalDeltaX = (targetDeltaX * TARGET_FORCE) + (avoidanceDeltaX * SEPARATION_FORCE);
    double finalDeltaY = (targetDeltaY * TARGET_FORCE) + (avoidanceDeltaY * SEPARATION_FORCE);
    
    // Normalize final movement
    double finalDistance = std::sqrt(finalDeltaX * finalDeltaX + finalDeltaY * finalDeltaY);
    if (finalDistance > 0.1) {
        finalDeltaX /= finalDistance;
        finalDeltaY /= finalDistance;
    }
    
    unit.getGameEntity().setPosition(currX + std::round(finalDeltaX), currY + std::round(finalDeltaY));
    return false;
}