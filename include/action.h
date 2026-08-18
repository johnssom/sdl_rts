#ifndef ACTION_H
#define ACTION_H

#include <vector>
#include <memory>

#include "obstacle.h"

class UnitEntity;

class Action {
    public:
        virtual bool updateUnit(UnitEntity& unit) = 0;
};

class MoveAction: public Action {
    private:
        int _targetX, _targetY;
        std::vector<std::shared_ptr<UnitEntity>>* _unitsPtr;
        std::vector<Obstacle>* _obstaclesPtr;
        static constexpr int SEPARATION_RADIUS = 80;
        static constexpr int MIN_SEPARATION_DISTANCE = 35;
        static constexpr double SEPARATION_FORCE = 0.8;
        static constexpr double TARGET_FORCE = 0.2;
        static constexpr int OBSTACLE_AVOIDANCE_RADIUS = 60;
        static constexpr double OBSTACLE_AVOIDANCE_FORCE = 1.0;

        bool isGroupAtTarget() const;
        double getGroupCentroidX() const;
        double getGroupCentroidY() const;
    public:
        MoveAction(int x, int y);
        MoveAction(int x, int y, std::vector<std::shared_ptr<UnitEntity>>* units);
        MoveAction(int x, int y, std::vector<std::shared_ptr<UnitEntity>>* units, std::vector<Obstacle>* obstacles);
        bool updateUnit(UnitEntity& unit);
        void calculateSeparation(UnitEntity& unit, double& outDeltaX, double& outDeltaY);
        void calculateObstacleAvoidance(UnitEntity& unit, double& outDeltaX, double& outDeltaY);
};

#endif