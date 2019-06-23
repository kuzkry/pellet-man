#include "enemy.h"

#include "node.h"
#include "player.h"

Enemy::Enemy(Player const& player, std::vector<Node> const& nodes, SpriteMap<MovementDirection> regularSprites, std::chrono::milliseconds const delayToLeaveHideout)
    : Character(nodes, rescalePixmaps(std::move(regularSprites)), initialPosition),
      player(player),
      frightenedSprites(rescalePixmaps(getFrightenedSprites())),
      delayToLeaveHideout(delayToLeaveHideout)
{
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    QObject::connect(&frightenedModeTimer, SIGNAL(timeout()), this, SLOT(disableRunawayState()));
    QObject::connect(&blinkingModeTimer, SIGNAL(timeout()), this, SLOT(blink()));
    QObject::connect(&initialDelayTimer, SIGNAL(timeout()), this, SLOT(releaseFromHideout()));
    QObject::connect(&animationTimer, SIGNAL(timeout()), this, SLOT(changeSprite()));
}

void Enemy::deinit()
{
    initialDelayTimer.stop();
    movementTimer.stop();
    frightenedModeTimer.stop();
    blinkingModeTimer.stop();
    animationTimer.stop();
}

void Enemy::init()
{
    currentDirection = MovementDirection::UP;
    deinit();
    setInitialPixmap(currentDirection);
    setInitialPosition();
    frightened = false;
    initialDelayTimer.start(delayToLeaveHideout);
    animationTimer.start(animationTime);
}

void Enemy::enableRunawayState()
{
    frightState = FrightState::INITIAL_BLUE;
    frightened = true;
    frightenedModeTimer.start(runAwayTime);
    blinkingModeTimer.start(runAwayTime - blinkingInterval);
}

auto Enemy::getFrightenedSprites() -> SpriteMap<FrightState>
{
    return {{FrightState::INITIAL_BLUE, {QPixmap(":/sprites/sprites/zombieghost1.png"), QPixmap(":/sprites/sprites/zombieghost2.png")}},
            {FrightState::TRANSFORMING_WHITE, {QPixmap(":/sprites/sprites/leavethisplace1.png"), QPixmap(":/sprites/sprites/leavethisplace1.png")}}};
}

auto Enemy::nextDirection(Node const& node) const -> MovementDirection
{
    std::vector<MovementDirection> possibleDirections;
    possibleDirections.reserve(node.movementPossibilities.size());
    for (auto const& [direction, isDirectionValid] : node.movementPossibilities)
    {
        if (isDirectionValid && direction != opposite(currentDirection))
            possibleDirections.push_back(direction);
    }

    return makeTurnDecision(possibleDirections);
}

auto Enemy::nextFrightState() const noexcept -> FrightState
{
    switch (frightState)
    {
    case FrightState::INITIAL_BLUE: return FrightState::TRANSFORMING_WHITE;
    case FrightState::TRANSFORMING_WHITE: return FrightState::INITIAL_BLUE;
    }
    return FrightState::INITIAL_BLUE;
}

template <typename Key>
auto Enemy::rescalePixmaps(SpriteMap<Key> spriteMap) -> SpriteMap<Key> {
    for (auto& value : spriteMap)
    {
        for (auto& pixmap : value.second)
            pixmap = pixmap.scaled(26, 26);
    }
    return spriteMap;
}

void Enemy::allowToMove()
{
    initialDelayTimer.stop();
    currentDirection = std::rand() % 2 ? MovementDirection::RIGHT : MovementDirection::LEFT;
    movementTimer.start(movementTime);
}

void Enemy::changeSprite()
{
    if (!frightened)
        setSprite(regularSprites, currentDirection);
    else
        setSprite(frightenedSprites, frightState);
}

void Enemy::move()
{
    auto const it = findCurrentNode();
    if (it != nodes.cend())
        currentDirection = nextDirection(*it);

    animate();
}

void Enemy::blink()
{
    frightState = nextFrightState();
    blinkingModeTimer.start(singleBlinkTime);
}

void Enemy::disableRunawayState()
{
    frightenedModeTimer.stop();
    blinkingModeTimer.stop();
    frightened = false;
}

void Enemy::releaseFromHideout()
{
    initialDelayTimer.start(movementTime);
    if (pos() == initialChasePoint)
        allowToMove();
    else
        animate();
}
