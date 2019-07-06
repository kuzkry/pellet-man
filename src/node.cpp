#include "node.h"

Node::Node(QPointF const point,
           bool possible_upward, bool possible_leftward,
           bool possible_downward, bool possible_rightward)
    : QPointF(point),
      possible_directions({{Direction::UP, possible_upward},
                             {Direction::LEFT, possible_leftward},
                             {Direction::DOWN, possible_downward},
                             {Direction::RIGHT, possible_rightward}}) {}
