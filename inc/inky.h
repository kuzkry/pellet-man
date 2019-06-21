#ifndef INKY_H
#define INKY_H

#include "enemy.h"
#include "blinky.h"

class Inky : public Enemy
{
    Q_OBJECT
public:
    Inky(Player const& player, std::vector<Node> const& nodes, Blinky const& blinky);

private:
    auto makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection override;
    void setInitialPixmap() override;

    Blinky const& blinky;
    static constexpr std::chrono::milliseconds delayToLeaveHideout{2600};

private slots:
    void allowToMove() override;
    void change() override;
};

#endif // INKY_H
