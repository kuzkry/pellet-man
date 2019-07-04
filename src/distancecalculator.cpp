#include "distancecalculator.h"

#include <cmath>
#include <functional>
#include <limits>

DistanceCalculator::DistanceCalculator(std::vector<MovementDirection> const& possible_directions, QPointF const current_pos, QPointF const target_pos)
    : possible_directions(possible_directions),
      current_pos(current_pos),
      target_pos(target_pos) {}

auto DistanceCalculator::calculate_shortest_direction() const -> MovementDirection
{
    qreal const min_distance = std::numeric_limits<qreal>::max();
    return calculate(min_distance, std::less<qreal>());
}

auto DistanceCalculator::calculate_longest_direction() const -> MovementDirection
{
    qreal const max_distance = std::numeric_limits<qreal>::min();
    return calculate(max_distance, std::greater<qreal>());
}

auto DistanceCalculator::calculate_distance(QPointF const from, QPointF const to) -> qreal
{
    return std::sqrt(std::pow(from.x() - to.x(), 2) + std::pow(from.y() - to.y(), 2));
}

auto DistanceCalculator::generate_next_position(QPointF const pos, MovementDirection const direction, qreal const offset) -> QPointF
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

template <typename Comp>
auto DistanceCalculator::calculate(qreal const boundary_value, Comp const comp) const -> MovementDirection
{
    MovementDirection ret = MovementDirection::UP;
    qreal prev_distance = boundary_value;
    for (MovementDirection const movement_direction : possible_directions)
    {
        qreal const distance = calculate_distance(generate_next_position(current_pos, movement_direction), target_pos);
        if (comp(distance, prev_distance))
        {
            prev_distance = distance;
            ret = movement_direction;
        }
    }
    return ret;
}

auto DistanceCalculator::generate_next_position(QPointF const pos, MovementDirection const direction) -> QPointF
{
    constexpr qreal SmallOffset = 1;
    return generate_next_position(pos, direction, SmallOffset);
}
