#ifndef BLINKY_H
#define BLINKY_H

#include "enemy.h"

class Blinky : public Enemy
{
public:
    Blinky(Player const& player, const std::vector<Node>& nodes);

private:
    static auto getRegularSprites() -> SpriteMap<MovementDirection>;
    auto makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection override;

    static constexpr std::chrono::milliseconds delayToLeaveHideout{1600};
};

#endif // BLINKY_H
