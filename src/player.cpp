#include "player.h"

#include "enemy.h"
#include "node.h"
#include "regularpellet.h"
#include "superpellet.h"

#include <QKeyEvent>

#include <algorithm>

Player::Player(std::vector<Node> const& nodes,
               std::vector<RegularPellet*> const& regular_pellets,
               std::vector<SuperPellet*> const& super_pellets,
               std::vector<Enemy*> const& enemies)
    : Character(nodes, get_sprites(), InitialPosition),
      regular_pellets(regular_pellets),
      super_pellets(super_pellets),
      enemies(enemies)
{
    QObject::connect(&initial_delay_timer, SIGNAL(timeout()), this, SLOT(allow_to_move()));
    QObject::connect(&movement_animation_timer, SIGNAL(timeout()), this, SLOT(animate_movement()));
    QObject::connect(&sprite_animation_timer, SIGNAL(timeout()), this, SLOT(animate_sprites()));
}

void Player::deinit()
{
    initial_delay_timer.stop();
    sprite_animation_timer.stop();
    movement_animation_timer.stop();
}

void Player::init()
{
    current_direction = pending_direction = Direction::LEFT;
    deinit();
    set_initial_pixmap(current_direction);
    set_initial_position();
    initial_delay_timer.start(InitialDelay);
    sprite_animation_timer.start(AnimationTime);
}

void Player::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
        pending_direction = Direction::LEFT;
        break;
    case Qt::Key_Right:
        pending_direction = Direction::RIGHT;
        break;
    case Qt::Key_Up:
        pending_direction = Direction::UP;
        break;
    case Qt::Key_Down:
        pending_direction = Direction::DOWN;
        break;
    case Qt::Key_Escape:
        emit interrupted();
        break;
    }
}

void Player::check_collisions()
{
    // get a list of all the items currently colliding with all pellets and enemies
    QList<QGraphicsItem*> const all_items = collidingItems();

    for (QGraphicsItem* const item : all_items)
    {
        if (auto const pellet = std::find(regular_pellets.cbegin(), regular_pellets.cend(), item); pellet != regular_pellets.cend())
        {
            emit regular_pellet_eaten(*pellet);
        }
        else if (auto const pellet = std::find(super_pellets.cbegin(), super_pellets.cend(), item); pellet != super_pellets.cend())
        {
            emit super_pellet_eaten(*pellet);
        }
        else if (auto const enemy = std::find(enemies.cbegin(), enemies.cend(), item); enemy != enemies.cend())
        {
            emit enemy_hit(*enemy);
        }
    }
}

auto Player::direction() const -> std::optional<Direction>
{
    if (auto const opposite_direction = try_opposite_direction(); opposite_direction.has_value())
        return opposite_direction;

    if (auto const it = find_current_node(); it != nodes.cend())
        return try_direction_at_node(*it);

    return current_direction;
}

auto Player::get_sprites() -> SpriteMap<Direction>
{
    return {{Direction::LEFT, {QPixmap(":/sprites/sprites/pacopenleft.png"), QPixmap(":/sprites/sprites/pacmidleft.png")}},
            {Direction::RIGHT, {QPixmap(":/sprites/sprites/pacopenright.png"), QPixmap(":/sprites/sprites/pacmidright.png")}},
            {Direction::UP, {QPixmap(":/sprites/sprites/pacopenup.png"), QPixmap(":/sprites/sprites/pacmidup.png")}},
            {Direction::DOWN, {QPixmap(":/sprites/sprites/pacopendown.png"), QPixmap(":/sprites/sprites/pacmiddown.png")}}};
}

auto Player::try_direction_at_node(Node const& node) const -> std::optional<Direction>
{
    std::vector<Direction> possible_directions;
    possible_directions.reserve(node.possible_directions.size());
    for (auto const& [direction, is_direction_valid] : node.possible_directions)
    {
        if (is_direction_valid)
            possible_directions.push_back(direction);
    }

    if (std::find(possible_directions.cbegin(), possible_directions.cend(), pending_direction) != possible_directions.cend()) // check if a pending move can be performed
        return pending_direction;
    else if (std::find(possible_directions.cbegin(), possible_directions.cend(), current_direction) != possible_directions.cend()) // check if Pac-Man can continue going in his current direction
        return current_direction;
    else // otherwise Pac-Man hits a wall
        return std::nullopt;
}

auto Player::try_opposite_direction() const -> std::optional<Direction>
{
    return pending_direction == opposite(current_direction) ? std::optional<Direction>(pending_direction) : std::nullopt;
}

void Player::allow_to_move()
{
    initial_delay_timer.stop();
    movement_animation_timer.start(MovementTime);
}

void Player::animate_movement()
{
    auto const next_direction = direction();

    if (next_direction.has_value())
    {
        current_direction = *next_direction;
        set_next_position();
    }

    check_collisions();
}

void Player::animate_sprites()
{
    set_sprite(regular_sprites, current_direction);
}
