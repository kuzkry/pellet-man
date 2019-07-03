#ifndef CLYDE_H
#define CLYDE_H

#include "enemy.h"

class Clyde : public Enemy
{
public:
    Clyde(Player const& player, std::vector<Node> const& nodes);

private:
    auto make_turn_decision(std::vector<MovementDirection> const& possible_movements) const -> MovementDirection override;

    static auto get_regular_sprites() -> SpriteMap<MovementDirection>;

    static constexpr std::chrono::milliseconds DelayToLeaveHideout{3100};
};

#endif // CLYDE_H
