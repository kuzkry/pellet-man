#include "enemy.h"

#include "node.h"
#include "player.h"

Enemy::Enemy(Player const& player, std::vector<Node> const& nodes, SpriteMap<MovementDirection> regular_sprites, std::chrono::milliseconds const delay_to_leave_hideout)
    : Character(nodes, rescale_pixmaps(std::move(regular_sprites)), InitialPosition),
      player(player),
      frightened_sprites(rescale_pixmaps(get_frightened_sprites())),
      delay_to_leave_hideout(delay_to_leave_hideout)
{
    QObject::connect(&movement_animation_timer, SIGNAL(timeout()), this, SLOT(animate_movement()));
    QObject::connect(&frightened_mode_timer, SIGNAL(timeout()), this, SLOT(disable_runaway_state()));
    QObject::connect(&blinking_mode_timer, SIGNAL(timeout()), this, SLOT(blink()));
    QObject::connect(&initial_delay_timer, SIGNAL(timeout()), this, SLOT(release_from_hideout()));
    QObject::connect(&sprite_animation_timer, SIGNAL(timeout()), this, SLOT(animate_sprites()));
}

void Enemy::deinit()
{
    initial_delay_timer.stop();
    movement_animation_timer.stop();
    frightened_mode_timer.stop();
    blinking_mode_timer.stop();
    sprite_animation_timer.stop();
}

void Enemy::init()
{
    deinit();
    disable_runaway_state();
    current_direction = MovementDirection::UP;
    set_initial_pixmap(current_direction);
    set_initial_position();
    initial_delay_timer.start(delay_to_leave_hideout);
    sprite_animation_timer.start(AnimationTime);
}

void Enemy::enable_runaway_state()
{
    fright_state = FrightState::INITIAL_BLUE;
    frightened_mode_timer.start(RunawayTime);
    blinking_mode_timer.start(RunawayTime - BlinkingInterval);
}

auto Enemy::is_frightened() const -> bool
{
    return frightened_mode_timer.isActive();
}

auto Enemy::get_frightened_sprites() -> SpriteMap<FrightState>
{
    return {{FrightState::INITIAL_BLUE, {QPixmap(":/sprites/sprites/zombieghost1.png"), QPixmap(":/sprites/sprites/zombieghost2.png")}},
            {FrightState::TRANSFORMING_WHITE, {QPixmap(":/sprites/sprites/leavethisplace1.png"), QPixmap(":/sprites/sprites/leavethisplace1.png")}}};
}

auto Enemy::direction() const -> MovementDirection
{
    auto const it = find_current_node();
    return it != nodes.cend() ? direction(*it) : current_direction;
}

auto Enemy::direction(Node const& node) const -> MovementDirection
{
    std::vector<MovementDirection> possible_directions;
    possible_directions.reserve(node.movement_possibilities.size());
    for (auto const& [direction, is_direction_valid] : node.movement_possibilities)
    {
        if (is_direction_valid && direction != opposite(current_direction))
            possible_directions.push_back(direction);
    }

    return make_turn_decision(possible_directions);
}

auto Enemy::next_fright_state() const noexcept -> FrightState
{
    switch (fright_state)
    {
    case FrightState::INITIAL_BLUE: return FrightState::TRANSFORMING_WHITE;
    case FrightState::TRANSFORMING_WHITE: return FrightState::INITIAL_BLUE;
    }
    return FrightState::INITIAL_BLUE;
}

template <typename Key>
auto Enemy::rescale_pixmaps(SpriteMap<Key> sprite_map) -> SpriteMap<Key> {
    for (auto& value : sprite_map)
    {
        for (auto& pixmap : value.second)
            pixmap = pixmap.scaled(26, 26);
    }
    return sprite_map;
}

void Enemy::allow_to_move()
{
    initial_delay_timer.stop();
    current_direction = std::rand() % 2 ? MovementDirection::RIGHT : MovementDirection::LEFT;
    movement_animation_timer.start(MovementTime);
}

void Enemy::animate_movement()
{
    current_direction = direction();
    set_next_position();
}

void Enemy::animate_sprites()
{
    if (!is_frightened())
        set_sprite(regular_sprites, current_direction);
    else
        set_sprite(frightened_sprites, fright_state);
}

void Enemy::blink()
{
    fright_state = next_fright_state();
    blinking_mode_timer.start(SingleBlinkTime);
}

void Enemy::disable_runaway_state()
{
    frightened_mode_timer.stop();
    blinking_mode_timer.stop();
    emit entered_chase_mode();
}

void Enemy::release_from_hideout()
{
    initial_delay_timer.start(MovementTime);
    if (pos() == InitialChasePoint)
        allow_to_move();
    else
        set_next_position();
}
