#ifndef DISTANCECALCULATOR_H
#define DISTANCECALCULATOR_H

#include "directions.h"

#include <QPointF>

#include <vector>

class DistanceCalculator {
public:
    DistanceCalculator(std::vector<MovementDirection> const& possible_directions, QPointF current_pos, QPointF target_pos);

    auto calculate_shortest_direction() const -> MovementDirection;
    auto calculate_longest_direction() const -> MovementDirection;

    static auto calculate_distance(QPointF from, QPointF to) -> qreal;
    static auto generate_next_position(QPointF pos, MovementDirection direction, qreal offset) -> QPointF;

private:
    template <typename Comp>
    auto calculate(qreal const boundary_value, Comp const comp) const -> MovementDirection;
    static auto generate_next_position(QPointF pos, MovementDirection direction) -> QPointF;

    std::vector<MovementDirection> const& possible_directions;
    QPointF const current_pos, target_pos;
};

#endif // DISTANCECALCULATOR_H
