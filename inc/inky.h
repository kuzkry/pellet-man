#ifndef INKY_H
#define INKY_H

#include "enemy.h"
#include "blinky.h"

class Inky : public Enemy
{
    Q_OBJECT
public:
    Inky(const Player& player, const std::vector<Node*>& nodes, const Blinky& blinky);
private:
    MovementDirection makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened);
    void startInitialDelayTimer()
    {
        initialDelayTimer.start(initialDelay);
    }
    void setInitialPixmap()
    {
        setPixmap(QPixmap(":/sprites/sprites/cghostU1.png").scaled(26, 26));
    }

    const Blinky& blinky;
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
