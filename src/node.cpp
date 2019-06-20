#include "node.h"

Node::Node(QPointF const point,
           bool possibleUpward, bool possibleLeftward,
           bool possibleDownward, bool possibleRightward)
    : QPointF(point),
      possibleUpward(possibleUpward),
      possibleLeftward(possibleLeftward),
      possibleDownward(possibleDownward),
      possibleRightward(possibleRightward) {}
