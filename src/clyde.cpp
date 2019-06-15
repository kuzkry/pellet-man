#include "clyde.h"
#include "player.h"
#include <cmath>
#include <cstdlib>

Clyde::Clyde(Player const& player, std::vector<Node*> const& nodes)
    : Enemy(player, nodes),
      initialDelay(3100)
{
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(change()));
    QObject::connect(&frightenedModeTimer, SIGNAL(timeout()), this, SLOT(disableRunawayState()));
    QObject::connect(&blinkingModeTimer, SIGNAL(timeout()), this, SLOT(blink()));
    init();
}

Clyde::MovementDirection Clyde::makeTurnDecision(
        std::map<MovementDirection, bool>& possibleMovements, bool frightened)
{
    int playerX = player.x(), playerY = player.y();
    unsigned int playerEnemyOffsetX = abs(playerX - x()), playerEnemyOffsetY = abs(playerY - y());
    DistanceAndDirectionBinder binder[4] = {
        {pow((playerY > y() ? playerEnemyOffsetY + 1 : playerEnemyOffsetY - 1), 2) + pow(playerEnemyOffsetX, 2), up},
        {pow((playerX > x() ? playerEnemyOffsetX + 1 : playerEnemyOffsetX - 1), 2) + pow(playerEnemyOffsetY, 2), left},
        {pow((playerY > y() ? playerEnemyOffsetY - 1 : playerEnemyOffsetY + 1), 2) + pow(playerEnemyOffsetX, 2), down},
        {pow((playerX > x() ? playerEnemyOffsetX - 1 : playerEnemyOffsetX + 1), 2) + pow(playerEnemyOffsetY, 2), right}};
    /* those directions are in the following order: up, left, down, right */

    {
        unsigned int nonChasingAreaLimiter = player.pixmap().width() * 8;
        unsigned int distanceFromPlayer = sqrt(pow(abs(playerX - x()), 2) + pow(abs(playerY - y()), 2));
        if(!frightened && nonChasingAreaLimiter < distanceFromPlayer)
        {
            std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                       sortDistanceAndDirectionBindersInAscendingOrder);
        }
        else
        {
            std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                       sortDistanceAndDirectionBindersInDescendingOrder);
        }
    }

    return chooseMostSuitableTurnOption(possibleMovements, binder);
}

void Clyde::allowToMove()
{
    initialDelayTimer.stop();
    QObject::disconnect(&initialDelayTimer, SIGNAL(timeout()), this, 0);
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    movementTimer.start(movementTime);
    moving = true;
    currentDirection = rand() % 2 ? right : left;
}

void Clyde::blink()
{
    blinking = !blinking;
    blinkingModeTimer.start(singleBlinkTime);
}

void Clyde::change()
{
    static bool phase = false;

    if(!frightened)
    {
        if(currentDirection == left)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/oghostL1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/oghostL2.png").scaled(26, 26));
        }
        else if(currentDirection == right)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/oghost1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/oghost2.png").scaled(26, 26));
        }
        else if(currentDirection == up)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/oghostU1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/oghostU2.png").scaled(26, 26));
        }
        else if(currentDirection == down)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/oghostD1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/oghostD2.png").scaled(26, 26));
        }
    }
    else
    {
        if (blinking || frightenedModeTimer.remainingTime() > blinkingInterval)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/zombieghost1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/zombieghost2.png").scaled(26, 26));
        }
        else
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/leavethisplace1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/leavethisplace2.png").scaled(26, 26));
        }
    }

    phase = !phase;
}

void Clyde::disableRunawayState()
{
    frightenedModeTimer.stop();
    blinkingModeTimer.stop();
    blinking = frightened = false;
}

void Clyde::move()
{
    checkPositionWithRespectToNodes();

    //moving a ghost
    switch(currentDirection)
    {
    case left:
        setPos(x() - 1, y());
        break;
    case right:
        setPos(x() + 1, y());
        break;
    case up:
        setPos(x(), y() - 1);
        break;
    case down:
        setPos(x(),y() + 1);
        break;
    }

    //teleporting on the edges of a map
    if(x() + this->pixmap().width() < 0) setPos(450, y());
    else if(x() > 450) setPos(-this->pixmap().width(), y());
}

void Clyde::releaseFromGhostHouse()
{
    initialDelayTimer.start(movementTime);
    if(y() == 168 && x() == 210)
    {
        allowToMove();
    }
    else
    {
        setPos(x(),y() - 1);
    }
}
