#include "node.h"

Node::Node(int x, int y,
           bool possibleUpward, bool possibleLeftward,
           bool possibleDownward, bool possibleRightward)
    : x(x),
      y(y),
      possibleUpward(possibleUpward),
      possibleLeftward(possibleLeftward),
      possibleDownward(possibleDownward),
      possibleRightward(possibleRightward) {}
