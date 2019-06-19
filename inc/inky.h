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
    MovementDirection makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) override;
    void startInitialDelayTimer() override;
    void setInitialPixmap() override;

    Blinky const& blinky;
    unsigned short int initialDelay;

private slots:
    void allowToMove() override;
    void blink() override;
    void change() override;
    void disableRunawayState() override;
    void move() override;
    void releaseFromGhostHouse() override;
};

#endif // INKY_H
