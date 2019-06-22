#include "enemy.h"

#include "node.h"

constexpr QPointF Enemy::initialChasePoint;

Enemy::Enemy(Player const& player, std::vector<Node> const& nodes, std::chrono::milliseconds const delayToLeaveHideout)
    : Character(nodes),
      player(player),
      movementTime(10),
      singleBlinkTime(20 * movementTime),
      blinkingInterval(2000),
      runAwayTime(8000),
      delayToLeaveHideout(delayToLeaveHideout) {}

void Enemy::checkPositionWithRespectToNodes()
{
    for (auto const& node : nodes)
    {
        if (isInNode(node))
        {
            std::map<MovementDirection, bool> movementPossibleFromTheNode;

            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(MovementDirection::UP, (currentDirection == MovementDirection::DOWN ? false : node.possibleUpward)));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(MovementDirection::LEFT, (currentDirection == MovementDirection::RIGHT ? false : node.possibleLeftward)));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(MovementDirection::DOWN, (currentDirection == MovementDirection::UP ? false : node.possibleDownward)));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(MovementDirection::RIGHT, (currentDirection == MovementDirection::LEFT ? false : node.possibleRightward)));

            currentDirection = makeTurnDecision(movementPossibleFromTheNode, false/*frightened*/);
            break;
        }
    }
}

void Enemy::disable()
{
    initialDelayTimer.stop();
    movementTimer.stop();
    frightenedModeTimer.stop();
    blinkingModeTimer.stop();
}

void Enemy::init()
{
    QObject::connect(&frightenedModeTimer, SIGNAL(timeout()), this, SLOT(disableRunawayState()));
    QObject::connect(&blinkingModeTimer, SIGNAL(timeout()), this, SLOT(blink()));
    disable();
    setInitialPixmap();
    setPos(210, 210);
    QObject::disconnect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    QObject::connect(&initialDelayTimer, SIGNAL(timeout()), this, SLOT(releaseFromHideout()));
    currentDirection = MovementDirection::UP;
    frightened = blinking = false;
    startInitialDelayTimer();
    movementTimer.start(movementTime);
}

void Enemy::enableRunawayState()
{
    frightened = true;
    frightenedModeTimer.start(runAwayTime);
    blinkingModeTimer.start(runAwayTime - blinkingInterval);
}

auto Enemy::chooseMostSuitableTurnOption(std::map<MovementDirection, bool>& possibleMovements,
                                         Enemy::DistanceAndDirectionBinder const* binder) const -> MovementDirection
{
    for (unsigned short i = 0; i < 4; ++i)
    {
        if (possibleMovements.find(binder[i].direction)->second)
            return binder[i].direction;
    }
    return MovementDirection::UP; // this is not going to be returned anyway
}

auto Enemy::sortDistanceAndDirectionBindersInAscendingOrder(void const* p1, void const* p2) -> int
{
    if (*(static_cast<DistanceAndDirectionBinder const*>(p1)) < *(static_cast<DistanceAndDirectionBinder const*>(p2)))
        return -1;
    else if (*(static_cast<DistanceAndDirectionBinder const*>(p1)) > *(static_cast<DistanceAndDirectionBinder const*>(p2)))
        return 1;

    return 0;
}

auto Enemy::sortDistanceAndDirectionBindersInDescendingOrder(void const* p1, void const* p2) -> int
{
    if (*(static_cast<DistanceAndDirectionBinder const*>(p1)) < *(static_cast<DistanceAndDirectionBinder const*>(p2)))
        return 1;
    else if (*(static_cast<DistanceAndDirectionBinder const*>(p1)) > *(static_cast<DistanceAndDirectionBinder const*>(p2)))
        return -1;

    return 0;
}

void Enemy::startInitialDelayTimer()
{
    initialDelayTimer.start(delayToLeaveHideout);
}

void Enemy::blink()
{
    blinking = !blinking;
    blinkingModeTimer.start(singleBlinkTime);
}

void Enemy::disableRunawayState()
{
    frightenedModeTimer.stop();
    blinkingModeTimer.stop();
    blinking = frightened = false;
}

void Enemy::move()
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

void Enemy::releaseFromHideout()
{
    initialDelayTimer.start(movementTime);
    if (pos() == initialChasePoint)
        allowToMove();
    else
        setPos(x(), y() - 1);
}
