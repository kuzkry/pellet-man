#ifndef CHARACTER_H
#define CHARACTER_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>

#include <vector>

struct Node;

class Character : public QObject, public QGraphicsPixmapItem
{
public:
    Character(std::vector<Node*> const& nodes) : nodes(nodes) {}
    ~Character() override = default;

protected:
    enum class MovementDirection{UP, LEFT, DOWN, RIGHT};

    virtual void checkPositionWithRespectToNodes() = 0;
    virtual void disable() = 0;
    virtual void init() = 0;

    auto isInNode(Node const& node) const -> bool;

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
