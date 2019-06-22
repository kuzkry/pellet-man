#ifndef BLINKY_H
#define BLINKY_H

#include "enemy.h"

class Blinky : public Enemy
{
    Q_OBJECT
public:
    Blinky(Player const& player, const std::vector<Node>& nodes);

private:
    auto makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection override;
    void setInitialPixmap() override;

    static constexpr std::chrono::milliseconds delayToLeaveHideout{1600};

private slots:
    void change() override;
};

#endif // BLINKY_H
