#include "pinky.h"
#include "player.h"
#include <cmath>
#include <cstdlib>

Pinky::Pinky(Player const& player, std::vector<Node*> const& nodes) : Enemy(player, nodes), initialDelay(2100)
{
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(change()));
    QObject::connect(&frightenedModeTimer, SIGNAL(timeout()), this, SLOT(disableRunawayState()));
    QObject::connect(&blinkingModeTimer, SIGNAL(timeout()), this, SLOT(blink()));
    init();
}

Pinky::MovementDirection Pinky::makeTurnDecision(
        std::map<MovementDirection, bool>& possibleMovements, bool frightened)
{
    int relativePlayerX = player.x(), relativePlayerY = player.y();

    if(!frightened)
    {
        unsigned int futurePlayerPositionOffset = player.pixmap().width() * 4;
        if(player.getCurrentDirection() == left) relativePlayerX -= futurePlayerPositionOffset;
        else if(player.getCurrentDirection() == right) relativePlayerX += futurePlayerPositionOffset;
        else if(player.getCurrentDirection() == up) relativePlayerY -= futurePlayerPositionOffset;
        else if(player.getCurrentDirection() == down) relativePlayerY += futurePlayerPositionOffset;
    }

    unsigned int playerEnemyOffsetX = abs(relativePlayerX - x()),
            playerEnemyOffsetY = abs(relativePlayerY - y());
    DistanceAndDirectionBinder binder[4] = {
        {pow((relativePlayerY > y() ? playerEnemyOffsetY + 1 : playerEnemyOffsetY - 1), 2) + pow(playerEnemyOffsetX, 2), up},
        {pow((relativePlayerX > x() ? playerEnemyOffsetX + 1 : playerEnemyOffsetX - 1), 2) + pow(playerEnemyOffsetY, 2), left},
        {pow((relativePlayerY > y() ? playerEnemyOffsetY - 1 : playerEnemyOffsetY + 1), 2) + pow(playerEnemyOffsetX, 2), down},
        {pow((relativePlayerX > x() ? playerEnemyOffsetX - 1 : playerEnemyOffsetX + 1), 2) + pow(playerEnemyOffsetY, 2), right}};
    /* those directions are in the following order: up, left, down, right */

    if(!frightened)
    {
        std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                   sortDistanceAndDirectionBindersInAscendingOrder);
    }
    else
    {
        std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                   sortDistanceAndDirectionBindersInDescendingOrder);
    }

    return chooseMostSuitableTurnOption(possibleMovements, binder);
}

void Pinky::allowToMove()
{
    initialDelayTimer.stop();
    QObject::disconnect(&initialDelayTimer, SIGNAL(timeout()), this, 0);
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    moving = true;
    currentDirection = rand() % 2 ? right : left;
}

void Pinky::blink()
{
    blinking = !blinking;
    blinkingModeTimer.start(singleBlinkTime);
}

void Pinky::change()
{
    static bool phase = false;

    if(!frightened)
    {
        if(currentDirection == left)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/pghostL1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/pghostL2.png").scaled(26, 26));
        }
        else if(currentDirection == right)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/pghost1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/pghost2.png").scaled(26, 26));
        }
        else if(currentDirection == up)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/pghostU1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/pghostU2.png").scaled(26, 26));
        }
        else if(currentDirection == down)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/pghostD1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/pghostD2.png").scaled(26, 26));
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

void Pinky::disableRunawayState()
{
    frightenedModeTimer.stop();
    blinkingModeTimer.stop();
    blinking = frightened = false;
}

void Pinky::move()
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

void Pinky::releaseFromGhostHouse()
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
