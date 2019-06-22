#include "node.h"

Node::Node(QPointF const point,
           bool possibleUpward, bool possibleLeftward,
           bool possibleDownward, bool possibleRightward)
    : QPointF(point),
      movementPossibilities({{MovementDirection::UP, possibleUpward},
                             {MovementDirection::LEFT, possibleLeftward},
                             {MovementDirection::DOWN, possibleDownward},
                             {MovementDirection::RIGHT, possibleRightward}}) {}
