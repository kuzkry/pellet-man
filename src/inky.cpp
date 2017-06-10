#include "inky.h"
#include <cmath>
#include <cstdlib>

Inky::Inky(const Player &player, const std::vector<Node*> &nodes, const Blinky &blinky)
    : Enemy(player, nodes), blinky(blinky), initialDelay(2600)
{
    initialDelayTimer = std::unique_ptr<QTimer>(new QTimer());
    movementTimer = std::unique_ptr<QTimer>(new QTimer());
    frightenedModeTimer = std::unique_ptr<QTimer>(new QTimer());
    blinkingModeTimer = std::unique_ptr<QTimer>(new QTimer());
    QObject::connect(movementTimer.get(), SIGNAL(timeout()), this, SLOT(change()));
    QObject::connect(frightenedModeTimer.get(), SIGNAL(timeout()), this, SLOT(disableRunawayState()));
    QObject::connect(blinkingModeTimer.get(), SIGNAL(timeout()), this, SLOT(blink()));
    init();
}

Inky::MovementDirection Inky::makeTurnDecision(
        std::map<MovementDirection, bool> &possibleMovements, bool frightened)
{
    int relativePlayerX = player.x(), relativePlayerY = player.y(), playerEnemyOffsetX, playerEnemyOffsetY;

    if(!frightened)
    {
        unsigned int futurePlayerPositionOffset = player.pixmap().width() * 2;
        if(player.getCurrentDirection() == left) relativePlayerX -= futurePlayerPositionOffset;
        else if(player.getCurrentDirection() == right) relativePlayerX += futurePlayerPositionOffset;
        else if(player.getCurrentDirection() == up) relativePlayerY -= futurePlayerPositionOffset;
        else if(player.getCurrentDirection() == down) relativePlayerY += futurePlayerPositionOffset;
        playerEnemyOffsetX = abs(relativePlayerX - blinky.x()) * 2;
        playerEnemyOffsetY = abs(relativePlayerY - blinky.y()) * 2;
    }
    else
    {
        playerEnemyOffsetX = abs(relativePlayerX - x());
        playerEnemyOffsetY = abs(relativePlayerY - y());
    }
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

void Inky::allowToMove()
{
    initialDelayTimer->stop();
    QObject::disconnect(initialDelayTimer.get(), SIGNAL(timeout()), this, 0);
    QObject::connect(movementTimer.get(), SIGNAL(timeout()), this, SLOT(move()));
    moving = true;
    currentDirection = rand() % 2 ? right : left;
}

void Inky::blink()
{
    blinking = !blinking;
    blinkingModeTimer->start(singleBlinkTime);
}

void Inky::change()
{
    static bool phase = false;

    if(!frightened)
    {
        if(currentDirection == left)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/cghostL1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/cghostL2.png").scaled(26, 26));
        }
        else if(currentDirection == right)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/cghost1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/cghost2.png").scaled(26, 26));
        }
        else if(currentDirection == up)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/cghostU1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/cghostU2.png").scaled(26, 26));
        }
        else if(currentDirection == down)
        {
            if(!phase) setPixmap(QPixmap(":/sprites/sprites/cghostD1.png").scaled(26, 26));
            else setPixmap(QPixmap(":/sprites/sprites/cghostD2.png").scaled(26, 26));
        }
    }
    else
    {
        if (blinking || frightenedModeTimer->remainingTime() > blinkingInterval)
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

void Inky::disableRunawayState()
{
    frightenedModeTimer->stop();
    blinkingModeTimer->stop();
    blinking = frightened = false;
}

void Inky::move()
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

void Inky::releaseFromGhostHouse()
{
    initialDelayTimer->start(movementTime);
    if(y() == 168 && x() == 210)
    {
        allowToMove();
    }
    else
    {
        setPos(x(),y() - 1);
    }
}
