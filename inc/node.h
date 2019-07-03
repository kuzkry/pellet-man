#ifndef NODE_H
#define NODE_H

#include "directions.h"

#include <QPointF>

#include <unordered_map>

struct Node : public QPointF
{
    Node(QPointF point,
         bool possible_upward, bool possible_leftward,
         bool possible_downward, bool possible_rightward);

    std::unordered_map<MovementDirection, bool> movement_possibilities;
};

#endif // NODE_H
