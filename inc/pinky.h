#ifndef PINKY_H
#define PINKY_H

#include "enemy.h"

class Pinky : public Enemy
{
    Q_OBJECT
public:
    Pinky(Player const& player, std::vector<Node> const& nodes);

private:
    auto makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection override;
    void setInitialPixmap() override;

    static constexpr std::chrono::milliseconds delayToLeaveHideout{2100};

private slots:
    void change() override;
};

#endif // PINKY_H
