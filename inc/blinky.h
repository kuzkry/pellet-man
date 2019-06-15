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
    void startInitialDelayTimer()
    {
        initialDelayTimer.start(initialDelay);
    }
    void setInitialPixmap()
    {
        setPixmap(QPixmap(":/sprites/sprites/rghostU1.png").scaled(26, 26));
    }

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
