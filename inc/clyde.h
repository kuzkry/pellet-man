#ifndef CLYDE_H
#define CLYDE_H

#include "enemy.h"

class Clyde : public Enemy
{
    Q_OBJECT
public:
    Clyde(Player const& player, std::vector<Node> const& nodes);

private:
    auto makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection override;
    void setInitialPixmap() override;

    static constexpr std::chrono::milliseconds delayToLeaveHideout{3100};

private slots:
    void allowToMove() override;
    void change() override;
};

#endif // CLYDE_H
