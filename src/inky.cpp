#include "inky.h"
#include "player.h"
#include <cmath>
#include <cstdlib>

Inky::Inky(Player const& player, std::vector<Node*> const& nodes, Blinky const& blinky)
    : Enemy(player, nodes),
      blinky(blinky),
      initialDelay(2600)
{
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(change()));
    QObject::connect(&frightenedModeTimer, SIGNAL(timeout()), this, SLOT(disableRunawayState()));
    QObject::connect(&blinkingModeTimer, SIGNAL(timeout()), this, SLOT(blink()));
    init();
}

Inky::MovementDirection Inky::makeTurnDecision(
        std::map<MovementDirection, bool>& possibleMovements, bool frightened)
{
    int relativePlayerX = player.x(),
        relativePlayerY = player.y(),
        playerEnemyOffsetX, playerEnemyOffsetY;

    if (!frightened)
    {
        unsigned int futurePlayerPositionOffset = player.pixmap().width() * 2;
        if (player.getCurrentDirection() == LEFT)
            relativePlayerX -= futurePlayerPositionOffset;
        else if (player.getCurrentDirection() == RIGHT)
            relativePlayerX += futurePlayerPositionOffset;
        else if (player.getCurrentDirection() == UP)
            relativePlayerY -= futurePlayerPositionOffset;
        else if (player.getCurrentDirection() == DOWN)
            relativePlayerY += futurePlayerPositionOffset;

        playerEnemyOffsetX = abs(relativePlayerX - blinky.x()) * 2;
        playerEnemyOffsetY = abs(relativePlayerY - blinky.y()) * 2;
    }
    else
    {
        playerEnemyOffsetX = abs(relativePlayerX - x());
        playerEnemyOffsetY = abs(relativePlayerY - y());
    }
    DistanceAndDirectionBinder binder[4] = {
        {pow((relativePlayerY > y() ? playerEnemyOffsetY + 1 : playerEnemyOffsetY - 1), 2) + pow(playerEnemyOffsetX, 2), UP},
        {pow((relativePlayerX > x() ? playerEnemyOffsetX + 1 : playerEnemyOffsetX - 1), 2) + pow(playerEnemyOffsetY, 2), LEFT},
        {pow((relativePlayerY > y() ? playerEnemyOffsetY - 1 : playerEnemyOffsetY + 1), 2) + pow(playerEnemyOffsetX, 2), DOWN},
        {pow((relativePlayerX > x() ? playerEnemyOffsetX - 1 : playerEnemyOffsetX + 1), 2) + pow(playerEnemyOffsetY, 2), RIGHT}};
    /* those directions are in the following order: up, left, down, right */

    if (!frightened)
        std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                   sortDistanceAndDirectionBindersInAscendingOrder);
    else
        std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                   sortDistanceAndDirectionBindersInDescendingOrder);

    return chooseMostSuitableTurnOption(possibleMovements, binder);
}

void Inky::allowToMove()
{
    initialDelayTimer.stop();
    QObject::disconnect(&initialDelayTimer, SIGNAL(timeout()), this, 0);
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    moving = true;
    currentDirection = rand() % 2 ? RIGHT : LEFT;
}

void Inky::blink()
{
    blinking = !blinking;
    blinkingModeTimer.start(singleBlinkTime);
}

void Inky::change()
{
    static bool phase = false;

    if (!frightened)
    {
        if (currentDirection == LEFT)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/cghostL1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/cghostL2.png").scaled(26, 26));
        }
        else if (currentDirection == RIGHT)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/cghost1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/cghost2.png").scaled(26, 26));
        }
        else if (currentDirection == UP)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/cghostU1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/cghostU2.png").scaled(26, 26));
        }
        else if (currentDirection == DOWN)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/cghostD1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/cghostD2.png").scaled(26, 26));
        }
    }
    else
    {
        if (blinking || frightenedModeTimer.remainingTime() > blinkingInterval)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/zombieghost1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/zombieghost2.png").scaled(26, 26));
        }
        else
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/leavethisplace1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/leavethisplace2.png").scaled(26, 26));
        }
    }

    phase = !phase;
}

void Inky::disableRunawayState()
{
    frightenedModeTimer.stop();
    blinkingModeTimer.stop();
    blinking = frightened = false;
}

void Inky::move()
{
    checkPositionWithRespectToNodes();

    //moving a ghost
    switch (currentDirection)
    {
    case LEFT:
        setPos(x() - 1, y());
        break;
    case RIGHT:
        setPos(x() + 1, y());
        break;
    case UP:
        setPos(x(), y() - 1);
        break;
    case DOWN:
        setPos(x(), y() + 1);
        break;
    }

    //teleporting on the edges of a map
    if (x() + pixmap().width() < 0)
        setPos(450, y());
    else if (x() > 450)
        setPos(-pixmap().width(), y());
}

void Inky::releaseFromGhostHouse()
{
    initialDelayTimer.start(movementTime);
    if (y() == 168 && x() == 210)
        allowToMove();
    else
        setPos(x(), y() - 1);
}
