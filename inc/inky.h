#ifndef INKY_H
#define INKY_H

#include "enemy.h"
#include "blinky.h"

class Inky : public Enemy
{
public:
    Inky(Player const& player, std::vector<Node> const& nodes, Blinky const& blinky);

private:
    auto make_turn_decision(std::vector<Direction> const& possible_directions) const -> Direction override;

    static auto get_regular_sprites() -> SpriteMap<Direction>;

    static constexpr std::chrono::milliseconds DelayToLeaveHideout{2600};

    Blinky const& blinky;
};

#endif // INKY_H
