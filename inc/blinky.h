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
    void startInitialDelayTimer() override;
    void setInitialPixmap() override;

    unsigned short initialDelay;

private slots:
    void allowToMove() override;
    void blink() override;
    void change() override;
    void disableRunawayState() override;
    void move() override;
    void releaseFromGhostHouse() override;
};

#endif // BLINKY_H
