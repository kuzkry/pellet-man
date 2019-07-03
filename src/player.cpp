#include "player.h"

#include "clyde.h"
#include "blinky.h"
#include "inky.h"
#include "lifecounter.h"
#include "node.h"
#include "pinky.h"
#include "regularpellet.h"
#include "score.h"
#include "superpellet.h"

#include <QFont>
#include <QGraphicsScene>
#include <QKeyEvent>

#include <algorithm>
#include <utility>
#include <vector>

Player::Player(std::vector<Node> const& nodes,
               Score& score,
               LifeCounter& life_counter,
               std::vector<RegularPellet*>& regular_pellets,
               std::vector<SuperPellet*>& super_pellets,
               std::function<void()> quit_callback,
               std::vector<Enemy*> const& enemies)
    : Character(nodes, get_sprites(), InitialPosition),
      score(score),
      life_counter(life_counter),
      regular_pellets(regular_pellets),
      super_pellets(super_pellets),
      quit_callback(std::move(quit_callback)),
      enemies(enemies)
{
    QObject::connect(&initial_delay_timer, SIGNAL(timeout()), this, SLOT(allow_to_move()));
    QObject::connect(&movement_timer, SIGNAL(timeout()), this, SLOT(move()));
    QObject::connect(&animation_timer, SIGNAL(timeout()), this, SLOT(change_sprite()));
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

void Player::deinit()
{
    initial_delay_timer.stop();
    animation_timer.stop();
    movement_timer.stop();
}

void Player::keyPressEvent(QKeyEvent* event)
{
    // move the player left and right
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
        prepare_to_end_game(QuitReason::PRESSED_ESC);
        break;
    }
}

void Player::check_collision_with_pellets_and_ghosts()
{
    // get a list of all the items currently colliding with all dots and enemies
    QList<QGraphicsItem*> all_items = collidingItems();

    // if one of the colliding items is Pac-Man, destroy that dot
    for (int i = 0; i < all_items.size(); ++i)
    {
        if (auto const it = std::find(regular_pellets.cbegin(), regular_pellets.cend(), all_items[i]); it != regular_pellets.cend())
        {
            score.little_increase(); // increase the score by 10

            // remove them from the scene (still on the heap)
            scene()->removeItem(*it);

            // delete them from the heap to save memory
            delete *it;

            // remove from a vector
            regular_pellets.erase(it);
        }
        else if (auto const it = std::find(super_pellets.cbegin(), super_pellets.cend(), all_items[i]); it != super_pellets.cend())
        {
            score.big_increase(); // increase the score by 50

            // remove them from the scene (still on the heap)
            scene()->removeItem(*it);

            // delete them from the heap to save memory
            delete *it;

            // remove from a vector
            super_pellets.erase(it);

            // checking if any of enemies is frightened (if not, that means that player will not get extra points
            if (!is_any_of_enemies_frightened())
                score.reset_multiplier();

            // frighten enemies - requires getting rid of const to call enable_runaway_state
            std::for_each(const_cast<std::vector<Enemy*>&>(enemies).begin(),
                          const_cast<std::vector<Enemy*>&>(enemies).end(),
                          [](Enemy* enemy){enemy->enable_runaway_state();});
        }
        else if (auto const enemy_it = std::find(enemies.cbegin(), enemies.cend(), all_items[i]); enemy_it != enemies.cend())
        {
            if (!(*enemy_it)->is_frightened())
            {
                life_counter.decrease();
                if (life_counter.get_lives() == 0)
                    prepare_to_end_game(QuitReason::DEFEAT);
                else
                {
                    std::for_each(enemies.cbegin(),
                                  enemies.cend(),
                                  [](Enemy* enemy){enemy->init();});
                    init();
                }
            }
            else
            {
                score.huge_increase();
                (*enemy_it)->init();
            }
        }
    }
    if (!regular_pellets.size() && !super_pellets.size())
        prepare_to_end_game(QuitReason::VICTORY);
}

auto Player::is_any_of_enemies_frightened() const -> bool
{
    return std::any_of(enemies.begin(), enemies.end(), [](Enemy const* enemy) { return enemy->is_frightened(); });
}

auto Player::get_sprites() -> SpriteMap<MovementDirection>
{
    return {{MovementDirection::LEFT, {QPixmap(":/sprites/sprites/pacopenleft.png"), QPixmap(":/sprites/sprites/pacmidleft.png")}},
            {MovementDirection::RIGHT, {QPixmap(":/sprites/sprites/pacopenright.png"), QPixmap(":/sprites/sprites/pacmidright.png")}},
            {MovementDirection::UP, {QPixmap(":/sprites/sprites/pacopenup.png"), QPixmap(":/sprites/sprites/pacmidup.png")}},
            {MovementDirection::DOWN, {QPixmap(":/sprites/sprites/pacopendown.png"), QPixmap(":/sprites/sprites/pacmiddown.png")}}};
}

void Player::prepare_to_end_game(Player::QuitReason reason) const
{
    QGraphicsTextItem* text = nullptr;
    switch (reason)
    {
    case QuitReason::PRESSED_ESC:
        end_game();
        return;
    case QuitReason::VICTORY:
        text = new QGraphicsTextItem("YOU WIN!");
        break;
    case QuitReason::DEFEAT:
        text = new QGraphicsTextItem("YOU LOSE!");
    }
    text->setPos(120, 210);
    text->setDefaultTextColor(Qt::red);
    text->setFont(QFont("times", 34));
    scene()->addItem(text);

    // disable all timers (disablesMovements)
    const_cast<Player*>(this)->deinit(); // another way is to make timers mutable
    std::for_each(const_cast<std::vector<Enemy*>&>(enemies).begin(),
                  const_cast<std::vector<Enemy*>&>(enemies).end(),
                  [](Enemy* enemy){enemy->deinit();});

    QTimer::singleShot(3000, this, SLOT(end_game()));
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

    check_collision_with_pellets_and_ghosts();
}

void Player::change_sprite()
{
    set_sprite(regular_sprites, current_direction);
}

void Player::end_game() const
{
    quit_callback();
}
