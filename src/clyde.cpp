#include "clyde.h"

#include "player.h"

#include <cmath>
#include <cstdlib>

Clyde::Clyde(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes),
      initialDelay(3100)
{
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(change()));
    QObject::connect(&frightenedModeTimer, SIGNAL(timeout()), this, SLOT(disableRunawayState()));
    QObject::connect(&blinkingModeTimer, SIGNAL(timeout()), this, SLOT(blink()));
    init();
}

auto Clyde::makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection
{
    int playerX = player.x(),
        playerY = player.y();
    unsigned playerEnemyOffsetX = std::abs(playerX - x()),
             playerEnemyOffsetY = std::abs(playerY - y());
    DistanceAndDirectionBinder binder[4] = {
        {std::pow((playerY > y() ? playerEnemyOffsetY + 1 : playerEnemyOffsetY - 1), 2) + std::pow(playerEnemyOffsetX, 2), MovementDirection::UP},
        {std::pow((playerX > x() ? playerEnemyOffsetX + 1 : playerEnemyOffsetX - 1), 2) + std::pow(playerEnemyOffsetY, 2), MovementDirection::LEFT},
        {std::pow((playerY > y() ? playerEnemyOffsetY - 1 : playerEnemyOffsetY + 1), 2) + std::pow(playerEnemyOffsetX, 2), MovementDirection::DOWN},
        {std::pow((playerX > x() ? playerEnemyOffsetX - 1 : playerEnemyOffsetX + 1), 2) + std::pow(playerEnemyOffsetY, 2), MovementDirection::RIGHT}};
    /* those directions are in the following order: up, left, down, right */

    {
        unsigned nonChasingAreaLimiter = player.pixmap().width() * 8,
                 distanceFromPlayer = sqrt(std::pow(std::abs(playerX - x()), 2) + std::pow(std::abs(playerY - y()), 2));
        if (!frightened && nonChasingAreaLimiter < distanceFromPlayer)
            std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                       sortDistanceAndDirectionBindersInAscendingOrder);
        else
            std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                       sortDistanceAndDirectionBindersInDescendingOrder);
    }

    return chooseMostSuitableTurnOption(possibleMovements, binder);
}

void Clyde::startInitialDelayTimer()
{
    initialDelayTimer.start(initialDelay);
}
void Clyde::setInitialPixmap()
{
    setPixmap(QPixmap(":/sprites/sprites/oghostU1.png").scaled(26, 26));
}

void Clyde::allowToMove()
{
    initialDelayTimer.stop();
    QObject::disconnect(&initialDelayTimer, SIGNAL(timeout()), this, 0);
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    movementTimer.start(movementTime);
    moving = true;
    currentDirection = std::rand() % 2 ? MovementDirection::RIGHT : MovementDirection::LEFT;
}

void Clyde::blink()
{
    blinking = !blinking;
    blinkingModeTimer.start(singleBlinkTime);
}

void Clyde::change()
{
    static bool phase = false;

    if (!frightened)
    {
        if (currentDirection == MovementDirection::LEFT)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/oghostL1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/oghostL2.png").scaled(26, 26));
        }
        else if (currentDirection == MovementDirection::RIGHT)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/oghost1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/oghost2.png").scaled(26, 26));
        }
        else if (currentDirection == MovementDirection::UP)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/oghostU1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/oghostU2.png").scaled(26, 26));
        }
        else if (currentDirection == MovementDirection::DOWN)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/oghostD1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/oghostD2.png").scaled(26, 26));
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
    switch (currentDirection)
    {
    case MovementDirection::LEFT:
        setPos(x() - 1, y());
        break;
    case MovementDirection::RIGHT:
        setPos(x() + 1, y());
        break;
    case MovementDirection::UP:
        setPos(x(), y() - 1);
        break;
    case MovementDirection::DOWN:
        setPos(x(), y() + 1);
        break;
    }

    //teleporting on the edges of a map
    if (x() + pixmap().width() < 0)
        setPos(450, y());
    else if (x() > 450)
        setPos(-pixmap().width(), y());
}

void Clyde::releaseFromGhostHouse()
{
    initialDelayTimer.start(movementTime);
    if (y() == 168 && x() == 210)
        allowToMove();
    else
        setPos(x(), y() - 1);
}
