#include "enemy.h"

void Enemy::checkPositionWithRespectToNodes()
{
    for(std::vector<Node*>::const_iterator it = nodes.cbegin(); it != nodes.cend(); it++)
    {
        if(isInNode(**it)) // red ghost is in the node
        {
            std::map<MovementDirection, bool> movementPossibleFromTheNode;

            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(up, (currentDirection == down ? false : (*it)->possibleUpward)));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(left, (currentDirection == right ? false : (*it)->possibleLeftward)));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(down, (currentDirection == up ? false : (*it)->possibleDownward)));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(right, (currentDirection == left ? false : (*it)->possibleRightward)));

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
    currentDirection = up;
    moving = frightened = blinking = false;
    startInitialDelayTimer();
    movementTimer.start(movementTime);
}

Character::MovementDirection Enemy::chooseMostSuitableTurnOption(
        std::map<MovementDirection, bool> &possibleMovements,
        const Enemy::DistanceAndDirectionBinder *binder) const
{
    for(unsigned short int i = 0; i < 4; ++i)
    {
        if(possibleMovements.find(binder[i].direction)->second)
        {
            return binder[i].direction;
        }
    }
    return MovementDirection(up); // this is not going to be returned anyway
}

int Enemy::sortDistanceAndDirectionBindersInAscendingOrder(const void *p1, const void *p2)
{
    if(*(reinterpret_cast<const DistanceAndDirectionBinder*>(p1)) < *(reinterpret_cast<const DistanceAndDirectionBinder*>(p2)))
    {
        return -1;
    }
    else if(*(reinterpret_cast<const DistanceAndDirectionBinder*>(p1)) > *(reinterpret_cast<const DistanceAndDirectionBinder*>(p2)))
    {
        return 1;
    }
    return 0;
}

int Enemy::sortDistanceAndDirectionBindersInDescendingOrder(const void *p1, const void *p2)
{
    if(*(reinterpret_cast<const DistanceAndDirectionBinder*>(p1)) < *(reinterpret_cast<const DistanceAndDirectionBinder*>(p2)))
    {
        return 1;
    }
    else if(*(reinterpret_cast<const DistanceAndDirectionBinder*>(p1)) > *(reinterpret_cast<const DistanceAndDirectionBinder*>(p2)))
    {
        return -1;
    }
    return 0;
}
