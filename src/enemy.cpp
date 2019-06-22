#include "enemy.h"

#include "node.h"

#include <cstdlib>

constexpr QPointF Enemy::initialChasePoint;
constexpr QSize Enemy::pixmapScaling;

Enemy::Enemy(Player const& player, std::vector<Node> const& nodes, SpriteMap<MovementDirection> regularSprites, std::chrono::milliseconds const delayToLeaveHideout)
    : Character(nodes),
      player(player),
      regularSprites(rescalePixmaps(std::move(regularSprites))),
      frightenedSprites(rescalePixmaps(getFrightenedSprites())),
      blinkingInterval(2000),
      movementTime(10),
      singleBlinkTime(20 * movementTime),
      runAwayTime(8000),
      delayToLeaveHideout(delayToLeaveHideout),
      spriteIndex(0) {
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(changeSprite()));
    QObject::connect(&frightenedModeTimer, SIGNAL(timeout()), this, SLOT(disableRunawayState()));
    QObject::connect(&blinkingModeTimer, SIGNAL(timeout()), this, SLOT(blink()));
    QObject::connect(&initialDelayTimer, SIGNAL(timeout()), this, SLOT(releaseFromHideout()));
}

void Enemy::init()
{
    deinit();
    setInitialPixmap();
    setPos(210, 210);
    QObject::disconnect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    currentDirection = MovementDirection::UP;
    frightened = false;
    initialDelayTimer.start(delayToLeaveHideout);
    movementTimer.start(movementTime);
}

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

void Enemy::deinit()
{
    initialDelayTimer.stop();
    movementTimer.stop();
    frightenedModeTimer.stop();
    blinkingModeTimer.stop();
}

void Enemy::enableRunawayState()
{
    frightState = FrightState::INITIAL_BLUE;
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

auto Enemy::getFrightenedSprites() -> SpriteMap<FrightState>
{
    return {{FrightState::INITIAL_BLUE, {QPixmap(":/sprites/sprites/zombieghost1.png"), QPixmap(":/sprites/sprites/zombieghost2.png")}},
            {FrightState::TRANSFORMING_WHITE, {QPixmap(":/sprites/sprites/leavethisplace1.png"), QPixmap(":/sprites/sprites/leavethisplace1.png")}}};
}

auto Enemy::nextFrightState() const noexcept -> FrightState
{
    switch (frightState) {
    case FrightState::INITIAL_BLUE: return FrightState::TRANSFORMING_WHITE;
    case FrightState::TRANSFORMING_WHITE: return FrightState::INITIAL_BLUE;
    }
    return FrightState::INITIAL_BLUE;
}

auto Enemy::nextSpriteIndex() const noexcept -> std::size_t
{
    return (spriteIndex + 1) % spriteCount;
}

template <typename Key>
auto Enemy::rescalePixmaps(SpriteMap<Key> spriteMap) -> SpriteMap<Key> {
    for (auto& value : spriteMap) {
        for (auto& pixmap : value.second)
            pixmap = pixmap.scaled(26, 26);
    }
    return spriteMap;
}

void Enemy::allowToMove()
{
    initialDelayTimer.stop();
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    currentDirection = std::rand() % 2 ? MovementDirection::RIGHT : MovementDirection::LEFT;
}

void Enemy::blink()
{
    frightState = nextFrightState();
    blinkingModeTimer.start(singleBlinkTime);
}

void Enemy::changeSprite()
{
    if (!frightened)
        setPixmap(regularSprites.find(currentDirection)->second[spriteIndex]);
    else
        setPixmap(frightenedSprites.find(frightState)->second[spriteIndex]);

    spriteIndex = nextSpriteIndex();
}

void Enemy::disableRunawayState()
{
    frightenedModeTimer.stop();
    blinkingModeTimer.stop();
    frightened = false;
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
