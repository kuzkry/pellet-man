#include "pinky.h"

#include "player.h"

#include <cmath>
#include <cstdlib>

Pinky::Pinky(Player const& player, std::vector<Node*> const& nodes)
    : Enemy(player, nodes),
      initialDelay(2100)
{
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(change()));
    QObject::connect(&frightenedModeTimer, SIGNAL(timeout()), this, SLOT(disableRunawayState()));
    QObject::connect(&blinkingModeTimer, SIGNAL(timeout()), this, SLOT(blink()));
    init();
}

auto Pinky::makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection
{
    int relativePlayerX = player.x(),
        relativePlayerY = player.y();

    if (!frightened)
    {
        unsigned int futurePlayerPositionOffset = player.pixmap().width() * 4;
        if (player.getCurrentDirection() == MovementDirection::LEFT)
            relativePlayerX -= futurePlayerPositionOffset;
        else if (player.getCurrentDirection() == MovementDirection::RIGHT)
            relativePlayerX += futurePlayerPositionOffset;
        else if (player.getCurrentDirection() == MovementDirection::UP)
            relativePlayerY -= futurePlayerPositionOffset;
        else if (player.getCurrentDirection() == MovementDirection::DOWN)
            relativePlayerY += futurePlayerPositionOffset;
    }

    unsigned int playerEnemyOffsetX = std::abs(relativePlayerX - x()),
                 playerEnemyOffsetY = std::abs(relativePlayerY - y());
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

void Pinky::startInitialDelayTimer()
{
    initialDelayTimer.start(initialDelay);
}
void Pinky::setInitialPixmap()
{
    setPixmap(QPixmap(":/sprites/sprites/pghostU1.png").scaled(26, 26));
}

void Pinky::allowToMove()
{
    initialDelayTimer.stop();
    QObject::disconnect(&initialDelayTimer, SIGNAL(timeout()), this, 0);
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    moving = true;
    currentDirection = std::rand() % 2 ? MovementDirection::RIGHT : MovementDirection::LEFT;
}

void Pinky::blink()
{
    blinking = !blinking;
    blinkingModeTimer.start(singleBlinkTime);
}

void Pinky::change()
{
    static bool phase = false;

    if (!frightened)
    {
        if (currentDirection == MovementDirection::LEFT)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/pghostL1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/pghostL2.png").scaled(26, 26));
        }
        else if (currentDirection == MovementDirection::RIGHT)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/pghost1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/pghost2.png").scaled(26, 26));
        }
        else if (currentDirection == MovementDirection::UP)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/pghostU1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/pghostU2.png").scaled(26, 26));
        }
        else if (currentDirection == MovementDirection::DOWN)
        {
            if (!phase)
                setPixmap(QPixmap(":/sprites/sprites/pghostD1.png").scaled(26, 26));
            else
                setPixmap(QPixmap(":/sprites/sprites/pghostD2.png").scaled(26, 26));
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
        setPos(x(), y () + 1);
        break;
    }

    //teleporting on the edges of a map
    if (x() + pixmap().width() < 0)
        setPos(450, y());
    else if (x() > 450)
        setPos(-pixmap().width(), y());
}

void Pinky::releaseFromGhostHouse()
{
    initialDelayTimer.start(movementTime);
    if (y() == 168 && x() == 210)
        allowToMove();
    else
        setPos(x(), y() - 1);
}
