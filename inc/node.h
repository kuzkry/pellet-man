#ifndef NODE_H
#define NODE_H

#include <QGraphicsRectItem>

struct Node : public QGraphicsRectItem
{
    Node(int x, int y,
         bool possibleUpward, bool possibleLeftward,
         bool possibleDownward, bool possibleRightward)
        : x(x),
          y(y),
          possibleUpward(possibleUpward),
          possibleLeftward(possibleLeftward),
          possibleDownward(possibleDownward),
          possibleRightward(possibleRightward) {}

    int x,y;
    bool possibleUpward, possibleLeftward, possibleDownward, possibleRightward;
};

#endif // NODE_H
