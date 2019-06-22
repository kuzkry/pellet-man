#ifndef CHARACTER_H
#define CHARACTER_H

#include "directions.h"

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPointF>
#include <QTimer>

#include <vector>

struct Node;

class Character : public QObject, public QGraphicsPixmapItem
{
public:
    Character(std::vector<Node> const& nodes, QPointF initialPosition);
    ~Character() override = default;

    virtual void init() = 0;

protected:
    virtual void checkPositionWithRespectToNodes() = 0;
    virtual void deinit() = 0;

    auto isInNode(Node const& node) const -> bool;
    void setInitialPosition();

    std::vector<Node> const& nodes;
    MovementDirection currentDirection;
    QTimer initialDelayTimer;
    QTimer movementTimer;
    QPointF const initialPosition;

protected slots:
    virtual void allowToMove() = 0;
    virtual void move() = 0;
};

#endif // CHARACTER_H
