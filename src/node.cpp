#include "node.h"

Node::Node(QPointF const point,
           bool possible_upward, bool possible_leftward,
           bool possible_downward, bool possible_rightward)
    : QPointF(point),
      movement_possibilities({{MovementDirection::UP, possible_upward},
                             {MovementDirection::LEFT, possible_leftward},
                             {MovementDirection::DOWN, possible_downward},
                             {MovementDirection::RIGHT, possible_rightward}}) {}
