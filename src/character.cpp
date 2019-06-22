#include "character.h"

#include "node.h"

Character::Character(std::vector<Node> const& nodes, QPointF const initialPosition)
    : nodes(nodes),
      initialPosition(initialPosition) {}

auto Character::isInNode(Node const& node) const -> bool
{
    return pos() == node;
}

void Character::setInitialPosition()
{
    setPos(initialPosition);
}
