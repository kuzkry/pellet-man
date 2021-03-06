#ifndef NODE_H
#define NODE_H

#include "directions.h"

#include <QPointF>

#include <unordered_map>

class Node : public QPointF
{
public:
    Node(QPointF point,
         bool possible_upward, bool possible_leftward,
         bool possible_downward, bool possible_rightward);

    std::unordered_map<Direction, bool> const possible_directions;
};

#endif // NODE_H
