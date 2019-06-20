#ifndef NODE_H
#define NODE_H

#include <QPointF>

struct Node : public QPointF
{
    Node(QPointF point,
         bool possibleUpward, bool possibleLeftward,
         bool possibleDownward, bool possibleRightward);

    bool possibleUpward, possibleLeftward, possibleDownward, possibleRightward;
};

#endif // NODE_H
