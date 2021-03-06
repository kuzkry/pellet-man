#include "clyde.h"

#include "distancecalculator.h"
#include "player.h"

Clyde::Clyde(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes, get_regular_sprites(), DelayToLeaveHideout) {}

auto Clyde::make_turn_decision(std::vector<Direction> const& possible_directions) const -> Direction
{
    DistanceCalculator const distance_calculator(possible_directions, pos(), player.pos());

    constexpr auto TilesAhead = 8;
    qreal const non_chasing_area = player.pixmap().width() * TilesAhead,
                distance_from_player = distance_calculator.calculate_distance(pos(), player.pos());

    return non_chasing_area < distance_from_player && !is_frightened() ? distance_calculator.calculate_shortest_direction() : distance_calculator.calculate_longest_direction();
}

auto Clyde::get_regular_sprites() -> SpriteMap<Direction>
{
    return {{Direction::LEFT, {QPixmap(":/sprites/sprites/oghostL1.png"), QPixmap(":/sprites/sprites/oghostL2.png")}},
            {Direction::RIGHT, {QPixmap(":/sprites/sprites/oghost1.png"), QPixmap(":/sprites/sprites/oghost2.png")}},
            {Direction::UP, {QPixmap(":/sprites/sprites/oghostU1.png"), QPixmap(":/sprites/sprites/oghostU2.png")}},
            {Direction::DOWN, {QPixmap(":/sprites/sprites/oghostD1.png"), QPixmap(":/sprites/sprites/oghostD2.png")}}};
}
