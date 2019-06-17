#ifndef PINKY_H
#define PINKY_H

#include "enemy.h"

class Pinky : public Enemy
{
    Q_OBJECT
public:
    Pinky(Player const& player, std::vector<Node*> const& nodes);

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

#endif // PINKY_H
