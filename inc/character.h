#ifndef CHARACTER_H
#define CHARACTER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <vector>
#include "node.h"

class Character : public QObject, public QGraphicsPixmapItem
{
public:
    Character(std::vector<Node*> const& nodes) : nodes(nodes) {}
    virtual ~Character() {}
protected:
    enum MovementDirection{up, left, down, right};

    virtual void checkPositionWithRespectToNodes() = 0;
    virtual void disable() = 0;
    virtual void init() = 0;
    bool isInNode(Node const& node)
    {
        if(x() == node.x && y() == node.y)
        {
            return true;
        }
        return false;
    }

    std::vector<Node*> const& nodes;
    MovementDirection currentDirection;
    QTimer initialDelayTimer;
    QTimer movementTimer;
    bool moving;
    // positions are already inherited (use x() or y())
protected slots:
    virtual void allowToMove() = 0;
    virtual void move() = 0;
};

/* anyway these virtual functions are going to be early bind and virtuality will not work on them
thus I put them in protected section */

#endif // CHARACTER_H
