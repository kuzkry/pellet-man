#include "enemy.h"

void Enemy::checkPositionWithRespectToNodes()
{
    for (std::vector<Node*>::const_iterator it = nodes.cbegin(); it != nodes.cend(); it++)
    {
        if (isInNode(**it)) // red ghost is in the node
        {
            std::map<MovementDirection, bool> movementPossibleFromTheNode;

            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(UP, (currentDirection == DOWN ? false : (*it)->possibleUpward)));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(LEFT, (currentDirection == RIGHT ? false : (*it)->possibleLeftward)));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(DOWN, (currentDirection == UP ? false : (*it)->possibleDownward)));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(RIGHT, (currentDirection == LEFT ? false : (*it)->possibleRightward)));

            currentDirection = makeTurnDecision(movementPossibleFromTheNode, false/*frightened*/);
            break;
        }
    }
}

void Enemy::enableRunawayState()
{
    frightened = true;
    frightenedModeTimer.start(runAwayTime);
    blinkingModeTimer.start(runAwayTime - blinkingInterval);
}

void Enemy::init()
{
    disable();
    setInitialPixmap();
    setPos(210, 210);
    QObject::disconnect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    QObject::connect(&initialDelayTimer, SIGNAL(timeout()), this, SLOT(releaseFromGhostHouse()));
    currentDirection = UP;
    moving = frightened = blinking = false;
    startInitialDelayTimer();
    movementTimer.start(movementTime);
}

Character::MovementDirection Enemy::chooseMostSuitableTurnOption(
        std::map<MovementDirection, bool>& possibleMovements,
        Enemy::DistanceAndDirectionBinder const* binder) const
{
    for (unsigned short int i = 0; i < 4; ++i)
    {
        if (possibleMovements.find(binder[i].direction)->second)
            return binder[i].direction;
    }
    return MovementDirection(UP); // this is not going to be returned anyway
}

int Enemy::sortDistanceAndDirectionBindersInAscendingOrder(void const* p1, void const* p2)
{
    if (*(static_cast<DistanceAndDirectionBinder const*>(p1)) < *(static_cast<DistanceAndDirectionBinder const*>(p2)))
        return -1;
    else if (*(static_cast<DistanceAndDirectionBinder const*>(p1)) > *(static_cast<DistanceAndDirectionBinder const*>(p2)))
        return 1;

    return 0;
}

int Enemy::sortDistanceAndDirectionBindersInDescendingOrder(void const* p1, void const* p2)
{
    if (*(static_cast<DistanceAndDirectionBinder const*>(p1)) < *(static_cast<DistanceAndDirectionBinder const*>(p2)))
        return 1;
    else if (*(static_cast<DistanceAndDirectionBinder const*>(p1)) > *(static_cast<DistanceAndDirectionBinder const*>(p2)))
        return -1;

    return 0;
}
