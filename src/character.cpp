#include "character.h"

#include "node.h"

auto Character::isInNode(Node const& node) const -> bool
{
    return x() == node.x && y() == node.y;
}
