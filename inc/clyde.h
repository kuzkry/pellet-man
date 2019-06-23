#ifndef CLYDE_H
#define CLYDE_H

#include "enemy.h"

class Clyde : public Enemy
{
public:
    Clyde(Player const& player, std::vector<Node> const& nodes);

private:
    auto makeTurnDecision(std::vector<MovementDirection> const& possibleMovements) const -> MovementDirection override;

    static auto getRegularSprites() -> SpriteMap<MovementDirection>;

    static constexpr std::chrono::milliseconds delayToLeaveHideout{3100};
};

#endif // CLYDE_H
