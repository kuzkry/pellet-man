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
    QObject::connect(&movement_timer, SIGNAL(timeout()), this, SLOT(move()));
    QObject::connect(&animation_timer, SIGNAL(timeout()), this, SLOT(change_sprite()));
}

void Player::deinit()
{
    initial_delay_timer.stop();
    animation_timer.stop();
    movement_timer.stop();
}

void Player::init()
{
    current_direction = pending_direction = MovementDirection::LEFT;
    deinit();
    set_initial_pixmap(current_direction);
    set_initial_position();
    moving = false;
    initial_delay_timer.start(InitialDelay);
    animation_timer.start(AnimationTime);
}

void Player::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
        pending_direction = MovementDirection::LEFT;
        break;
    case Qt::Key_Right:
        pending_direction = MovementDirection::RIGHT;
        break;
    case Qt::Key_Up:
        pending_direction = MovementDirection::UP;
        break;
    case Qt::Key_Down:
        pending_direction = MovementDirection::DOWN;
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

auto Player::get_sprites() -> SpriteMap<MovementDirection>
{
    return {{MovementDirection::LEFT, {QPixmap(":/sprites/sprites/pacopenleft.png"), QPixmap(":/sprites/sprites/pacmidleft.png")}},
            {MovementDirection::RIGHT, {QPixmap(":/sprites/sprites/pacopenright.png"), QPixmap(":/sprites/sprites/pacmidright.png")}},
            {MovementDirection::UP, {QPixmap(":/sprites/sprites/pacopenup.png"), QPixmap(":/sprites/sprites/pacmidup.png")}},
            {MovementDirection::DOWN, {QPixmap(":/sprites/sprites/pacopendown.png"), QPixmap(":/sprites/sprites/pacmiddown.png")}}};
}

void Player::set_movement(MovementDirection const new_direction) noexcept
{
    current_direction = new_direction;
    moving = true;
}

void Player::set_movement_in_node(Node const& node)
{
    std::vector<MovementDirection> possible_directions;
    possible_directions.reserve(node.movement_possibilities.size());
    for (auto const& [direction, is_direction_valid] : node.movement_possibilities)
    {
        if (is_direction_valid)
            possible_directions.push_back(direction);
    }

    if (std::find(possible_directions.cbegin(), possible_directions.cend(), pending_direction) != possible_directions.cend()) // check if a pending move can be performed
        set_movement(pending_direction);
    else if (std::find(possible_directions.cbegin(), possible_directions.cend(), current_direction) != possible_directions.cend()) // check if Pac-Man can continue going in his current direction
        set_movement(current_direction);
    else // otherwise Pac-Man hits a wall
        stop();
}

void Player::stop() noexcept
{
    moving = false;
}

void Player::try_to_set_opposite_movement() noexcept
{
    if (pending_direction == opposite(current_direction))
        set_movement(pending_direction);
}

void Player::allow_to_move()
{
    initial_delay_timer.stop();
    movement_timer.start(MovementTime);
    moving = true;
}

void Player::move()
{
    auto const it = find_current_node();
    if (it != nodes.cend())
        set_movement_in_node(*it);
    else
        try_to_set_opposite_movement();

    if (moving)
        animate();

    check_collisions();
}

void Player::change_sprite()
{
    set_sprite(regular_sprites, current_direction);
}
