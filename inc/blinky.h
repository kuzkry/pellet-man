#ifndef BLINKY_H
#define BLINKY_H

#include "enemy.h"

class Blinky : public Enemy
{
    Q_OBJECT
public:
    Blinky(Player const& player, const std::vector<Node*>& nodes);

private:
    MovementDirection makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened);
    void startInitialDelayTimer();
    void setInitialPixmap();

    unsigned short int initialDelay;

private slots:
    void allowToMove();
    void blink();
    void change();
    void disableRunawayState();
    void move();
    void releaseFromGhostHouse();
};

#endif // BLINKY_H
