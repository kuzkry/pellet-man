#ifndef INKY_H
#define INKY_H

#include "enemy.h"
#include "blinky.h"

class Inky : public Enemy
{
    Q_OBJECT
public:
    Inky(Player const& player, std::vector<Node*> const& nodes, Blinky const& blinky);

private:
    MovementDirection makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened);
    void startInitialDelayTimer();
    void setInitialPixmap();

    Blinky const& blinky;
    unsigned short int initialDelay;

private slots:
    void allowToMove();
    void blink();
    void change();
    void disableRunawayState();
    void move();
    void releaseFromGhostHouse();
};

#endif // INKY_H
