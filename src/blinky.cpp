#include "blinky.h"

#include "distancecalculator.h"
#include "player.h"

Blinky::Blinky(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes, get_regular_sprites(), DelayToLeaveHideout) {}

auto Blinky::make_turn_decision(std::vector<MovementDirection> const& possible_movements) const -> MovementDirection
{
    DistanceCalculator const distance_calculator(possible_movements, pos(), player.pos());

    return !is_frightened() ? distance_calculator.calculate_shortest_direction() : distance_calculator.calculate_longest_direction();
}

auto Blinky::get_regular_sprites() -> SpriteMap<MovementDirection>
{
    return {{MovementDirection::LEFT, {QPixmap(":/sprites/sprites/rghostL1.png"), QPixmap(":/sprites/sprites/rghostL2.png")}},
            {MovementDirection::RIGHT, {QPixmap(":/sprites/sprites/rghost1.png"), QPixmap(":/sprites/sprites/rghost2.png")}},
            {MovementDirection::UP, {QPixmap(":/sprites/sprites/rghostU1.png"), QPixmap(":/sprites/sprites/rghostU2.png")}},
            {MovementDirection::DOWN, {QPixmap(":/sprites/sprites/rghostD1.png"), QPixmap(":/sprites/sprites/rghostD2.png")}}};
}
