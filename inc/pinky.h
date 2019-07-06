#ifndef PINKY_H
#define PINKY_H

#include "enemy.h"

class Pinky : public Enemy
{
public:
    Pinky(Player const& player, std::vector<Node> const& nodes);

private:
    auto make_turn_decision(std::vector<Direction> const& possible_directions) const -> Direction override;

    static auto get_regular_sprites() -> SpriteMap<Direction>;

    static constexpr std::chrono::milliseconds DelayToLeaveHideout{2100};
};

#endif // PINKY_H
