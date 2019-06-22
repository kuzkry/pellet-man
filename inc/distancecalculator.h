#ifndef DISTANCECALCULATOR_H
#define DISTANCECALCULATOR_H

#include "directions.h"

#include <QPointF>

#include <vector>

class DistanceCalculator {
public:
    DistanceCalculator(std::vector<MovementDirection> const& possibleDirections, QPointF currentPos, QPointF targetPos);

    auto calculateShortestDirection() const -> MovementDirection;
    auto calculateLongestDirection() const -> MovementDirection;

    static auto calculateDistance(QPointF from, QPointF to) -> qreal;
    static auto generateNextPosition(QPointF pos, MovementDirection direction, qreal offset) -> QPointF;

private:
    template <typename Comp>
    auto calculate(qreal const boundaryValue, Comp const comp) const -> MovementDirection;
    static auto generateNextPosition(QPointF pos, MovementDirection direction) -> QPointF;

    std::vector<MovementDirection> const& possibleDirections;
    QPointF const currentPos, targetPos;
};

#endif // DISTANCECALCULATOR_H
