#ifndef ENEMY_H
#define ENEMY_H

#include "character.h"

#include <QSize>

#include <array>
#include <chrono>
#include <unordered_map>

class Player;

class Enemy : public Character
{
    Q_OBJECT
protected:
    static constexpr std::size_t spriteCount = 2;

    template <typename Key>
    using SpriteMap = std::unordered_map<Key, std::array<QPixmap, spriteCount>>;

public:
    Enemy(Player const& player, std::vector<Node> const& nodes, SpriteMap<MovementDirection> regularSprites, std::chrono::milliseconds delayToLeaveHideout);

    void deinit() override;
    void init() override;

    void enableRunawayState();
    auto isFrightened() const noexcept -> bool;

protected:
    enum class FrightState {INITIAL_BLUE, TRANSFORMING_WHITE};

    virtual auto makeTurnDecision(std::vector<MovementDirection> const& possibleMovements) const -> MovementDirection = 0;

    Player const& player;

private:
    static constexpr QPointF initialPosition = {210, 210};
    static constexpr QPointF initialChasePoint = {210, 168};
    static constexpr QSize pixmapScaling = {26, 26};
    static constexpr std::chrono::milliseconds movementTime{10};
    static constexpr std::chrono::milliseconds singleBlinkTime{20 * movementTime};
    static constexpr std::chrono::milliseconds blinkingInterval{2000};
    static constexpr std::chrono::milliseconds runAwayTime{8000};

    static auto getFrightenedSprites() -> SpriteMap<FrightState>;
    auto nextDirection(Node const& node) const -> MovementDirection;
    auto nextFrightState() const noexcept -> FrightState;
    auto nextSpriteIndex() const noexcept -> std::size_t;
    template <typename Key>
    static auto rescalePixmaps(SpriteMap<Key> spriteMap) -> SpriteMap<Key>;
    void setInitialPixmap();

    QTimer blinkingModeTimer, frightenedModeTimer;
    SpriteMap<MovementDirection> const regularSprites;
    SpriteMap<FrightState> const frightenedSprites;
    std::chrono::milliseconds const delayToLeaveHideout;
    FrightState frightState;
    std::size_t spriteIndex = 0;
    bool frightened;

private slots:
    void allowToMove() override;
    void move() override;

    void blink();
    void changeSprite();
    void disableRunawayState();
    void releaseFromHideout();
};

inline auto Enemy::isFrightened() const noexcept -> bool
{
    return frightened;
}

#endif // ENEMY_H
