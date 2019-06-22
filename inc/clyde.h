#ifndef CLYDE_H
#define CLYDE_H

#include "enemy.h"

class Clyde : public Enemy
{
public:
    Clyde(Player const& player, std::vector<Node> const& nodes);

private:
    static auto getRegularSprites() -> SpriteMap<MovementDirection>;
    auto makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection override;

    static constexpr std::chrono::milliseconds delayToLeaveHideout{3100};
};

#endif // CLYDE_H
