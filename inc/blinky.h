#ifndef BLINKY_H
#define BLINKY_H

#include "enemy.h"

class Blinky : public Enemy
{
public:
    Blinky(Player const& player, const std::vector<Node>& nodes);

private:
    auto makeTurnDecision(std::vector<MovementDirection> const& possibleMovements) const -> MovementDirection override;

    static auto getRegularSprites() -> SpriteMap<MovementDirection>;

    static constexpr std::chrono::milliseconds delayToLeaveHideout{1600};
};

#endif // BLINKY_H
