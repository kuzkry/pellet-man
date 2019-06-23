#ifndef PINKY_H
#define PINKY_H

#include "enemy.h"

class Pinky : public Enemy
{
public:
    Pinky(Player const& player, std::vector<Node> const& nodes);

private:
    static auto getRegularSprites() -> SpriteMap<MovementDirection>;
    auto makeTurnDecision(std::vector<MovementDirection> const& possibleMovements) const -> MovementDirection override;

    static constexpr std::chrono::milliseconds delayToLeaveHideout{2100};
};

#endif // PINKY_H
