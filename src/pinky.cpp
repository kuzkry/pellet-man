#include "pinky.h"

#include "distancecalculator.h"
#include "player.h"

Pinky::Pinky(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes, get_regular_sprites(), DelayToLeaveHideout) {}

auto Pinky::make_turn_decision(std::vector<Direction> const& possible_directions) const -> Direction
{
    if (!is_frightened())
    {
        constexpr auto TilesAhead = 4;
        qreal const  player_pos_offset = player.pixmap().width() * TilesAhead;
        auto const future_player_position = DistanceCalculator::generate_next_position(player.pos(), player.get_current_direction(), player_pos_offset);

        DistanceCalculator const distance_calculator(possible_directions, pos(), future_player_position);
        return distance_calculator.calculate_shortest_direction();
    }
    return DistanceCalculator(possible_directions, pos(), player.pos()).calculate_longest_direction();
}

auto Pinky::get_regular_sprites() -> SpriteMap<Direction>
{
    return {{Direction::LEFT, {QPixmap(":/sprites/sprites/pghostL1.png"), QPixmap(":/sprites/sprites/pghostL2.png")}},
            {Direction::RIGHT, {QPixmap(":/sprites/sprites/pghost1.png"), QPixmap(":/sprites/sprites/pghost2.png")}},
            {Direction::UP, {QPixmap(":/sprites/sprites/pghostU1.png"), QPixmap(":/sprites/sprites/pghostU2.png")}},
            {Direction::DOWN, {QPixmap(":/sprites/sprites/pghostD1.png"), QPixmap(":/sprites/sprites/pghostD2.png")}}};
}
