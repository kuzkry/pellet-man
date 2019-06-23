#include "character.h"

#include "node.h"

Character::Character(std::vector<Node> const& nodes, QPointF const initialPosition)
    : nodes(nodes),
      initialPosition(initialPosition) {}

void Character::animate()
{
    switch (currentDirection)
    {
    case MovementDirection::LEFT:
        setPos(x() - 1, y());
        break;
    case MovementDirection::RIGHT:
        setPos(x() + 1, y());
        break;
    case MovementDirection::UP:
        setPos(x(), y() - 1);
        break;
    case MovementDirection::DOWN:
        setPos(x(), y() + 1);
        break;
    }

    teleportOnMapEdge();
}

auto Character::findCurrentNode() const -> std::vector<Node>::const_iterator
{
    return std::find_if(nodes.cbegin(), nodes.cend(), [this](auto& node) { return isInNode(node); });
}

void Character::setInitialPosition()
{
    setPos(initialPosition);
}

auto Character::isInNode(Node const& node) const -> bool
{
    return pos() == node;
}

void Character::teleportOnMapEdge()
{
    if (x() + pixmap().width() < 0)
        setPos(450, y());
    else if (x() > 450)
        setPos(-pixmap().width(), y());
}
