#include "inky.h"

#include "player.h"

#include <cmath>
#include <cstdlib>

constexpr std::chrono::milliseconds Inky::delayToLeaveHideout;

Inky::Inky(Player const& player, std::vector<Node> const& nodes, Blinky const& blinky)
    : Enemy(player, nodes, delayToLeaveHideout),
      blinky(blinky)
{
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(change()));
    init();
}

auto Inky::makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection
{
    int relativePlayerX = player.x(),
        relativePlayerY = player.y(),
        playerEnemyOffsetX, playerEnemyOffsetY;

    if (!frightened)
    {
        unsigned futurePlayerPositionOffset = player.pixmap().width() * 2;
        switch (player.getCurrentDirection()) {
        case MovementDirection::LEFT:
            relativePlayerX -= futurePlayerPositionOffset;
            break;
        case MovementDirection::RIGHT:
            relativePlayerX += futurePlayerPositionOffset;
            break;
        case MovementDirection::UP:
            relativePlayerY -= futurePlayerPositionOffset;
            break;
        case MovementDirection::DOWN:
            relativePlayerY += futurePlayerPositionOffset;
            break;
        }

        playerEnemyOffsetX = std::abs(relativePlayerX - blinky.x()) * 2;
        playerEnemyOffsetY = std::abs(relativePlayerY - blinky.y()) * 2;
    }
    else
    {
        playerEnemyOffsetX = std::abs(relativePlayerX - x());
        playerEnemyOffsetY = std::abs(relativePlayerY - y());
    }
    DistanceAndDirectionBinder binder[4] = {
        {std::pow((relativePlayerY > y() ? playerEnemyOffsetY + 1 : playerEnemyOffsetY - 1), 2) + std::pow(playerEnemyOffsetX, 2), MovementDirection::UP},
        {std::pow((relativePlayerX > x() ? playerEnemyOffsetX + 1 : playerEnemyOffsetX - 1), 2) + std::pow(playerEnemyOffsetY, 2), MovementDirection::LEFT},
        {std::pow((relativePlayerY > y() ? playerEnemyOffsetY - 1 : playerEnemyOffsetY + 1), 2) + std::pow(playerEnemyOffsetX, 2), MovementDirection::DOWN},
        {std::pow((relativePlayerX > x() ? playerEnemyOffsetX - 1 : playerEnemyOffsetX + 1), 2) + std::pow(playerEnemyOffsetY, 2), MovementDirection::RIGHT}};
    /* those directions are in the following order: up, left, down, right */

    if (!frightened)
        std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                   sortDistanceAndDirectionBindersInAscendingOrder);
    else
        std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                   sortDistanceAndDirectionBindersInDescendingOrder);

    return chooseMostSuitableTurnOption(possibleMovements, binder);
}

void Inky::setInitialPixmap()
{
    setPixmap(QPixmap(":/sprites/sprites/cghostU1.png").scaled(26, 26));
}

void Inky::allowToMove()
{
    initialDelayTimer.stop();
    QObject::disconnect(&initialDelayTimer, SIGNAL(timeout()), this, 0);
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    currentDirection = std::rand() % 2 ? MovementDirection::RIGHT : MovementDirection::LEFT;
}

void Inky::change()
{
    static bool phase = false;

    if (!frightened)
    {
        switch (currentDirection) {
        case MovementDirection::LEFT:
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/cghostL1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/cghostL2.png").scaled(26, 26));
            break;
        case MovementDirection::RIGHT:
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/cghost1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/cghost2.png").scaled(26, 26));
            break;
        case MovementDirection::UP:
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/cghostU1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/cghostU2.png").scaled(26, 26));
            break;
        case MovementDirection::DOWN:
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/cghostD1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/cghostD2.png").scaled(26, 26));
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
