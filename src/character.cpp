#include "character.h"

#include "gamewindow.h"
#include "node.h"

#include <utility>

Character::Character(std::vector<Node> const& nodes, SpriteMap<MovementDirection> regularSprites, QPointF const initialPosition)
    : nodes(nodes),
      regularSprites(std::move(regularSprites)),
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

void Character::setInitialPixmap(MovementDirection const direction)
{
    setPixmap(regularSprites.find(direction)->second[0]);
}

void Character::setInitialPosition()
{
    setPos(initialPosition);
}

auto Character::isInNode(Node const& node) const -> bool
{
    return pos() == node;
}

auto Character::nextSpriteIndex() const noexcept -> std::size_t
{
    return (spriteIndex + 1) % spriteCount;
}

void Character::teleportOnMapEdge()
{
    if (x() + pixmap().width() < 0)
        setPos(gameWindow.width(), y());
    else if (x() > gameWindow.width())
        setPos(-pixmap().width(), y());
}
