#include "inky.h"

#include "distancecalculator.h"
#include "player.h"

#include <cmath>
#include <utility>

Inky::Inky(Player const& player, std::vector<Node> const& nodes, Blinky const& blinky)
    : Enemy(player, nodes, get_regular_sprites(), DelayToLeaveHideout),
      blinky(blinky) {}

auto Inky::make_turn_decision(std::vector<Direction> const& possible_directions) const -> Direction
{
    if (!is_frightened())
    {
        constexpr auto TilesAhead = 2;
        qreal const player_pos_offset = player.pixmap().width() * TilesAhead;
        auto const future_player_pos = DistanceCalculator::generate_next_position(player.pos(), player.get_current_direction(), player_pos_offset);
        auto const blinky_pos = blinky.pos();
        auto const euclidean_vector = std::make_pair(std::abs(future_player_pos.x() - blinky_pos.x()) * 2,
                                                     std::abs(future_player_pos.y() - blinky_pos.y()) * 2);
        qreal const target_position_x = blinky_pos.x() + (blinky_pos.x() < future_player_pos.x() ? euclidean_vector.first : -euclidean_vector.first),
                    target_position_y = blinky_pos.y() + (blinky_pos.y() < future_player_pos.y() ? euclidean_vector.second : -euclidean_vector.second);
        QPointF const target_position(target_position_x, target_position_y);

        DistanceCalculator const distance_calculator(possible_directions, pos(), target_position);
        return distance_calculator.calculate_shortest_direction();
    }
    return DistanceCalculator(possible_directions, pos(), player.pos()).calculate_longest_direction();
}

auto Inky::get_regular_sprites() -> SpriteMap<Direction>
{
    return {{Direction::LEFT, {QPixmap(":/sprites/sprites/cghostL1.png"), QPixmap(":/sprites/sprites/cghostL2.png")}},
            {Direction::RIGHT, {QPixmap(":/sprites/sprites/cghost1.png"), QPixmap(":/sprites/sprites/cghost2.png")}},
            {Direction::UP, {QPixmap(":/sprites/sprites/cghostU1.png"), QPixmap(":/sprites/sprites/cghostU2.png")}},
            {Direction::DOWN, {QPixmap(":/sprites/sprites/cghostD1.png"), QPixmap(":/sprites/sprites/cghostD2.png")}}};
}
