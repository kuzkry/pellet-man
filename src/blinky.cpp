#include "blinky.h"

#include "distancecalculator.h"
#include "player.h"

Blinky::Blinky(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes, get_regular_sprites(), DelayToLeaveHideout) {}

auto Blinky::make_turn_decision(std::vector<Direction> const& possible_directions) const -> Direction
{
    DistanceCalculator const distance_calculator(possible_directions, pos(), player.pos());

    return !is_frightened() ? distance_calculator.calculate_shortest_direction() : distance_calculator.calculate_longest_direction();
}

auto Blinky::get_regular_sprites() -> SpriteMap<Direction>
{
    return {{Direction::LEFT, {QPixmap(":/sprites/sprites/rghostL1.png"), QPixmap(":/sprites/sprites/rghostL2.png")}},
            {Direction::RIGHT, {QPixmap(":/sprites/sprites/rghost1.png"), QPixmap(":/sprites/sprites/rghost2.png")}},
            {Direction::UP, {QPixmap(":/sprites/sprites/rghostU1.png"), QPixmap(":/sprites/sprites/rghostU2.png")}},
            {Direction::DOWN, {QPixmap(":/sprites/sprites/rghostD1.png"), QPixmap(":/sprites/sprites/rghostD2.png")}}};
}
