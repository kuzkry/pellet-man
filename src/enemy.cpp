#include "enemy.h"

#include "node.h"
#include "player.h"

Enemy::Enemy(Player const& player, std::vector<Node> const& nodes, SpriteMap<MovementDirection> regularSprites, std::chrono::milliseconds const delayToLeaveHideout)
    : Character(nodes, initialPosition),
      player(player),
      regularSprites(rescalePixmaps(std::move(regularSprites))),
      frightenedSprites(rescalePixmaps(getFrightenedSprites())),
      delayToLeaveHideout(delayToLeaveHideout)
{
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(changeSprite()));
    QObject::connect(&frightenedModeTimer, SIGNAL(timeout()), this, SLOT(disableRunawayState()));
    QObject::connect(&blinkingModeTimer, SIGNAL(timeout()), this, SLOT(blink()));
    QObject::connect(&initialDelayTimer, SIGNAL(timeout()), this, SLOT(releaseFromHideout()));
}

void Enemy::deinit()
{
    initialDelayTimer.stop();
    movementTimer.stop();
    frightenedModeTimer.stop();
    blinkingModeTimer.stop();
}

void Enemy::init()
{
    deinit();
    setInitialPixmap();
    setInitialPosition();
    QObject::disconnect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    currentDirection = MovementDirection::UP;
    frightened = false;
    initialDelayTimer.start(delayToLeaveHideout);
    movementTimer.start(movementTime);
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

auto Enemy::nextSpriteIndex() const noexcept -> std::size_t
{
    return (spriteIndex + 1) % spriteCount;
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

void Enemy::setInitialPixmap()
{
    setPixmap(regularSprites.find(MovementDirection::UP)->second[0]);
}

void Enemy::allowToMove()
{
    initialDelayTimer.stop();
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    currentDirection = std::rand() % 2 ? MovementDirection::RIGHT : MovementDirection::LEFT;
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

void Enemy::releaseFromHideout()
{
    initialDelayTimer.start(movementTime);
    if (pos() == initialChasePoint)
        allowToMove();
    else
        animate();
}
