#ifndef NODE_H
#define NODE_H

#include "directions.h"

#include <QPointF>

#include <unordered_map>

class Node : public QPointF
{
public:
    Node(QPointF point,
         bool possibleUpward, bool possibleLeftward,
         bool possibleDownward, bool possibleRightward);

    std::unordered_map<MovementDirection, bool> movementPossibilities;
};

#endif // NODE_H
