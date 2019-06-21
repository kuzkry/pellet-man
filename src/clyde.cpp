#include "clyde.h"

#include "player.h"

#include <cmath>
#include <cstdlib>

constexpr std::chrono::milliseconds Clyde::delayToLeaveHideout;

Clyde::Clyde(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes, delayToLeaveHideout)
{
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(change()));
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

void Clyde::change()
{
    static bool phase = false;

    if (!frightened)
    {
        switch (currentDirection) {
        case MovementDirection::LEFT:
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/oghostL1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/oghostL2.png").scaled(26, 26));
            break;
        case MovementDirection::RIGHT:
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/oghost1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/oghost2.png").scaled(26, 26));
            break;
        case MovementDirection::UP:
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/oghostU1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/oghostU2.png").scaled(26, 26));
            break;
        case MovementDirection::DOWN:
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/oghostD1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/oghostD2.png").scaled(26, 26));
            break;
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
