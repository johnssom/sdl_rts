#ifndef ACTION_H
#define ACTION_H

#include <vector>
#include <memory>
#include <utility>

#include "obstacle.h"

class UnitEntity;

class Action {
    public:
        virtual bool updateUnit(UnitEntity& unit) = 0;
        virtual ~Action() {}
};

class MoveAction: public Action {
    private:
        int _targetX, _targetY;
        std::vector<std::shared_ptr<UnitEntity>>* _unitsPtr;
        std::vector<Obstacle>* _obstaclesPtr;

        static constexpr double BOID_RADIUS = 100.0;
        static constexpr double SEPARATION_RADIUS = 50.0;
        static constexpr double SEPARATION_WEIGHT = 1.5;
        static constexpr double ALIGNMENT_WEIGHT = 0.3;
        static constexpr double COHESION_WEIGHT = 0.15;
        static constexpr double TARGET_WEIGHT = 0.6;
        static constexpr double OBSTACLE_AVOIDANCE_RADIUS = 60.0;
        static constexpr double OBSTACLE_AVOIDANCE_WEIGHT = 2.0;
        static constexpr double MAX_SPEED = 1.0;

        bool isGroupAtTarget() const;
        double getGroupCentroidX() const;
        double getGroupCentroidY() const;
    public:
        MoveAction(int x, int y);
        MoveAction(int x, int y, std::vector<std::shared_ptr<UnitEntity>>* units);
        MoveAction(int x, int y, std::vector<std::shared_ptr<UnitEntity>>* units, std::vector<Obstacle>* obstacles);
        bool updateUnit(UnitEntity& unit);
        void calculateSeparation(UnitEntity& unit, double& outX, double& outY);
        void calculateAlignment(UnitEntity& unit, double& outX, double& outY);
        void calculateCohesion(UnitEntity& unit, double& outX, double& outY);
        void calculateObstacleAvoidance(UnitEntity& unit, double& outX, double& outY);
};

class PathAction: public Action {
    private:
        std::vector<std::pair<int, int>> _path;
        int _currentWaypoint;
        std::vector<std::shared_ptr<UnitEntity>>* _unitsPtr;

        static constexpr double SEPARATION_RADIUS = 50.0;
        static constexpr double SEPARATION_WEIGHT = 2.0;
        static constexpr double WAYPOINT_WEIGHT = 0.6;
        static constexpr double MAX_SPEED = 1.0;
        static constexpr int WAYPOINT_REACH_DIST = 8;

        void calculateSeparation(UnitEntity& unit, double& outX, double& outY);
    public:
        PathAction(std::vector<std::pair<int, int>> path, std::vector<std::shared_ptr<UnitEntity>>* units);
        bool updateUnit(UnitEntity& unit);
};

#endif
