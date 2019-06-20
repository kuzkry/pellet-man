#include "character.h"

#include "node.h"

auto Character::isInNode(Node const& node) const -> bool
{
    return pos() == node;
}
