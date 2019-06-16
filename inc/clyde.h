#ifndef CLYDE_H
#define CLYDE_H

#include "enemy.h"

class Clyde : public Enemy
{
    Q_OBJECT
public:
    Clyde(Player const& player, std::vector<Node*> const& nodes);

private:
    MovementDirection makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened);
    void startInitialDelayTimer()
    {
        initialDelayTimer.start(initialDelay);
    }
    void setInitialPixmap()
    {
        setPixmap(QPixmap(":/sprites/sprites/oghostU1.png").scaled(26, 26));
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

#endif // CLYDE_H
