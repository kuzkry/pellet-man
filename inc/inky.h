#ifndef INKY_H
#define INKY_H

#include "enemy.h"
#include "blinky.h"

class Inky : public Enemy
{
public:
    Inky(Player const& player, std::vector<Node> const& nodes, Blinky const& blinky);

private:
    static auto getRegularSprites() -> SpriteMap<MovementDirection>;
    auto makeTurnDecision(std::vector<MovementDirection> const& possibleMovements) const -> MovementDirection override;

    Blinky const& blinky;
    static constexpr std::chrono::milliseconds delayToLeaveHideout{2600};
};

#endif // INKY_H
