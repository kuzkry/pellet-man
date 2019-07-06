#ifndef CHARACTER_H
#define CHARACTER_H

#include "directions.h"

#include <QGraphicsPixmapItem>
#include <QPointF>
#include <QTimer>

#include <array>
#include <unordered_map>
#include <vector>

class Node;

class Character : public QGraphicsPixmapItem
{
protected:
    static constexpr unsigned SpriteCount = 2;

    template <typename Key>
    using SpriteMap = std::unordered_map<Key, std::array<QPixmap, SpriteCount>>;

public:
    Character(std::vector<Node> const& nodes, SpriteMap<MovementDirection> regular_sprites, QPointF initial_position);
    ~Character() override = default;

    virtual void init() = 0;

protected:
    virtual void deinit() = 0;

    void animate();
    template <typename Key>
    void set_sprite(SpriteMap<Key> const& sprite_map, Key key);
    auto find_current_node() const -> std::vector<Node>::const_iterator;
    void set_initial_pixmap(MovementDirection direction);
    void set_initial_position();

    QTimer movement_timer, animation_timer, initial_delay_timer;
    SpriteMap<MovementDirection> const regular_sprites;
    QPointF const initial_position;
    std::vector<Node> const& nodes;
    MovementDirection current_direction;

protected slots:
    virtual void allow_to_move() = 0;
    virtual void change_sprite() = 0;
    virtual void move() = 0;

private:
    auto is_in_node(Node const& node) const -> bool;
    auto next_sprite_index() const noexcept -> unsigned;
    void teleport_on_map_edge();

    unsigned sprite_index = 0;
};

template <typename Key>
inline void Character::set_sprite(SpriteMap<Key> const& sprite_map, Key const key)
{
    setPixmap(sprite_map.find(key)->second[sprite_index]);
    sprite_index = next_sprite_index();
}

#endif // CHARACTER_H
