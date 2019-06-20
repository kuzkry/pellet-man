#include "blinky.h"

#include "player.h"

#include <cmath>
#include <cstdlib>

Blinky::Blinky(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes),
      initialDelay(1600)
{
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(change()));
    QObject::connect(&frightenedModeTimer, SIGNAL(timeout()), this, SLOT(disableRunawayState()));
    QObject::connect(&blinkingModeTimer, SIGNAL(timeout()), this, SLOT(blink()));
    init();
}

auto Blinky::makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection
{
    unsigned playerEnemyOffsetX = std::abs(player.x() - x()),
             playerEnemyOffsetY = std::abs(player.y() - y());
    DistanceAndDirectionBinder binder[4] = {
        {std::pow((player.y() > y() ? playerEnemyOffsetY + 1 : playerEnemyOffsetY - 1), 2) + std::pow(playerEnemyOffsetX, 2), MovementDirection::UP},
        {std::pow((player.x() > x() ? playerEnemyOffsetX + 1 : playerEnemyOffsetX - 1), 2) + std::pow(playerEnemyOffsetY, 2), MovementDirection::LEFT},
        {std::pow((player.y() > y() ? playerEnemyOffsetY - 1 : playerEnemyOffsetY + 1), 2) + std::pow(playerEnemyOffsetX, 2), MovementDirection::DOWN},
        {std::pow((player.x() > x() ? playerEnemyOffsetX - 1 : playerEnemyOffsetX + 1), 2) + std::pow(playerEnemyOffsetY, 2), MovementDirection::RIGHT}};
    /* those directions are in the following order: up, left, down, right */

    if (!frightened)
        std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                   sortDistanceAndDirectionBindersInAscendingOrder);
    else
        std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                   sortDistanceAndDirectionBindersInDescendingOrder);

    return chooseMostSuitableTurnOption(possibleMovements, binder);
}

void Blinky::startInitialDelayTimer()
{
    initialDelayTimer.start(initialDelay);
}
void Blinky::setInitialPixmap()
{
    setPixmap(QPixmap(":/sprites/sprites/rghostU1.png").scaled(26, 26));
}

void Blinky::allowToMove()
{
    initialDelayTimer.stop();
    QObject::disconnect(&initialDelayTimer, SIGNAL(timeout()), this, 0);
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    moving = true;
    currentDirection = std::rand() % 2 ? MovementDirection::RIGHT : MovementDirection::LEFT;
}

void Blinky::blink()
{
    blinking = !blinking;
    blinkingModeTimer.start(singleBlinkTime);
}

void Blinky::change()
{
    static bool phase = false;

    if (!frightened)
    {
        switch (currentDirection) {
        case MovementDirection::LEFT:
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/rghostL1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/rghostL2.png").scaled(26, 26));
            break;
        case MovementDirection::RIGHT:
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/rghost1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/rghost2.png").scaled(26, 26));
            break;
        case MovementDirection::UP:
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/rghostU1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/rghostU2.png").scaled(26, 26));
            break;
        case MovementDirection::DOWN:
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/rghostD1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/rghostD2.png").scaled(26, 26));
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

void Blinky::disableRunawayState()
{
    frightenedModeTimer.stop();
    blinkingModeTimer.stop();
    blinking = frightened = false;
}

void Blinky::move()
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

void Blinky::releaseFromGhostHouse()
{
    initialDelayTimer.start(movementTime);
    if (y() == 168 && x() == 210)
        allowToMove();
    else
        setPos(x(), y() - 1);
}
