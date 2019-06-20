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

#endif // PINKY_H
