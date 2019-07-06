#include "character.h"

#include "gameconstants.h"
#include "node.h"

#include <utility>

Character::Character(std::vector<Node> const& nodes, SpriteMap<MovementDirection> regular_sprites, QPointF const initial_position)
    : regular_sprites(std::move(regular_sprites)),
      initial_position(initial_position),
      nodes(nodes) {}

void Character::animate()
{
    switch (current_direction)
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

    teleport_on_map_edge();
}

auto Character::find_current_node() const -> std::vector<Node>::const_iterator
{
    return std::find_if(nodes.cbegin(), nodes.cend(), [this](auto& node) { return is_in_node(node); });
}

void Character::set_initial_pixmap(MovementDirection const direction)
{
    setPixmap(regular_sprites.find(direction)->second[0]);
}

void Character::set_initial_position()
{
    setPos(initial_position);
}

auto Character::is_in_node(Node const& node) const -> bool
{
    return pos() == node;
}

auto Character::next_sprite_index() const noexcept -> unsigned
{
    return (sprite_index + 1) % SpriteCount;
}

void Character::teleport_on_map_edge()
{
    if (x() + pixmap().width() < 0)
        setPos(GameWindow.width(), y());
    else if (x() > GameWindow.width())
        setPos(-pixmap().width(), y());
}
