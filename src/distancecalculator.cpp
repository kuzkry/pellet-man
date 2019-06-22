#include "distancecalculator.h"

#include <cmath>
#include <functional>
#include <limits>

DistanceCalculator::DistanceCalculator(std::vector<MovementDirection> const& possibleDirections, QPointF const currentPos, QPointF const targetPos)
    : possibleDirections(possibleDirections),
      currentPos(currentPos),
      targetPos(targetPos) {}

auto DistanceCalculator::calculateShortestDirection() const -> MovementDirection
{
    qreal const min_distance = std::numeric_limits<qreal>::max();
    return calculate(min_distance, std::less<qreal>());
}

auto DistanceCalculator::calculateLongestDirection() const -> MovementDirection
{
    qreal const max_distance = std::numeric_limits<qreal>::min();
    return calculate(max_distance, std::greater<qreal>());
}

auto DistanceCalculator::calculateDistance(QPointF const from, QPointF const to) -> qreal
{
    return std::sqrt(std::pow(from.x() - to.x(), 2) + std::pow(from.y() - to.y(), 2));
}

template <typename Comp>
auto DistanceCalculator::calculate(qreal const boundaryValue, Comp const comp) const -> MovementDirection
{
    MovementDirection ret = MovementDirection::UP;
    qreal prev_distance = boundaryValue;
    for (MovementDirection const movementDirection : possibleDirections)
    {
        qreal const distance = calculateDistance(generateNextPosition(currentPos, movementDirection), targetPos);
        if (comp(distance, prev_distance))
        {
            prev_distance = distance;
            ret = movementDirection;
        }
    }
    return ret;
}

auto DistanceCalculator::generateNextPosition(QPointF const pos, MovementDirection const direction, qreal const offset) -> QPointF
{
    switch (direction)
    {
    case MovementDirection::UP: return {pos.x(), pos.y() - offset};
    case MovementDirection::DOWN: return {pos.x(), pos.y() + offset};
    case MovementDirection::LEFT: return {pos.x() - offset, pos.y()};
    case MovementDirection::RIGHT: return {pos.x() + offset, pos.y()};
    }
    return pos;
}

auto DistanceCalculator::generateNextPosition(QPointF const pos, MovementDirection const direction) -> QPointF
{
    constexpr qreal smallOffset = 1;
    return generateNextPosition(pos, direction, smallOffset);
}
