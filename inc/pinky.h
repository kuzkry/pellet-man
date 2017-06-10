#ifndef PINKY_H
#define PINKY_H

#include "enemy.h"

class Pinky : public Enemy
{
    Q_OBJECT
public:
    Pinky(const Player &player, const std::vector<Node*> &nodes);
private:
    MovementDirection makeTurnDecision(std::map<MovementDirection, bool> &possibleMovements, bool frightened);
    void startInitialDelayTimer()
    {
        initialDelayTimer->start(initialDelay);
    }
    void setInitialPixmap()
    {
        setPixmap(QPixmap(":/sprites/sprites/pghostU1.png").scaled(26, 26));
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

#endif // PINKY_H
