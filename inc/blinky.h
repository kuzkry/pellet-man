#ifndef BLINKY_H
#define BLINKY_H

#include "enemy.h"

class Blinky : public Enemy
{
public:
    Blinky(Player const& player, const std::vector<Node>& nodes);

private:
    auto make_turn_decision(std::vector<MovementDirection> const& possible_movements) const -> MovementDirection override;

    static auto get_regular_sprites() -> SpriteMap<MovementDirection>;

    static constexpr std::chrono::milliseconds DelayToLeaveHideout{1600};
};

#endif // BLINKY_H
