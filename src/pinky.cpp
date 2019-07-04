#include "pinky.h"

#include "distancecalculator.h"
#include "player.h"

Pinky::Pinky(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes, get_regular_sprites(), DelayToLeaveHideout) {}

auto Pinky::make_turn_decision(std::vector<MovementDirection> const& possible_movements) const -> MovementDirection
{
    if (!is_frightened())
    {
        constexpr auto TilesAhead = 4;
        qreal const  player_pos_offset = player.pixmap().width() * TilesAhead;
        auto const future_player_position = DistanceCalculator::generate_next_position(player.pos(), player.get_current_direction(), player_pos_offset);

        DistanceCalculator const distance_calculator(possible_movements, pos(), future_player_position);
        return distance_calculator.calculate_shortest_direction();
    }
    return DistanceCalculator(possible_movements, pos(), player.pos()).calculate_longest_direction();
}

auto Pinky::get_regular_sprites() -> SpriteMap<MovementDirection>
{
    return {{MovementDirection::LEFT, {QPixmap(":/sprites/sprites/pghostL1.png"), QPixmap(":/sprites/sprites/pghostL2.png")}},
            {MovementDirection::RIGHT, {QPixmap(":/sprites/sprites/pghost1.png"), QPixmap(":/sprites/sprites/pghost2.png")}},
            {MovementDirection::UP, {QPixmap(":/sprites/sprites/pghostU1.png"), QPixmap(":/sprites/sprites/pghostU2.png")}},
            {MovementDirection::DOWN, {QPixmap(":/sprites/sprites/pghostD1.png"), QPixmap(":/sprites/sprites/pghostD2.png")}}};
}
