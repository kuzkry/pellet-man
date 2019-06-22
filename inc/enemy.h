#ifndef ENEMY_H
#define ENEMY_H

#include "character.h"

#include <QSize>

#include <array>
#include <chrono>
#include <map>
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
    Enemy(Player const& player, std::vector<Node> const& nodes,  SpriteMap<MovementDirection> regularSprites, std::chrono::milliseconds delayToLeaveHideout);

    void checkPositionWithRespectToNodes() override;
    void deinit() override;
    void init() override;

    void enableRunawayState();
    auto isFrightened() const -> bool
    {
        return frightened;
    }

protected:
    enum class FrightState {INITIAL_BLUE, TRANSFORMING_WHITE};

    struct DistanceAndDirectionBinder
    {
        DistanceAndDirectionBinder(double distance, MovementDirection direction)
            : distance(distance),
              direction(direction) {}
        auto operator<(DistanceAndDirectionBinder const& ref) const -> bool
        {
            return distance < ref.distance;
        }
        auto operator>(DistanceAndDirectionBinder const& ref) const -> bool
        {
            return !(*this < ref);
        }

        float const distance;
        MovementDirection const direction;
    };

    virtual auto makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection = 0;

    auto chooseMostSuitableTurnOption(std::map<MovementDirection, bool>& possibleMovements,
                                      DistanceAndDirectionBinder const* binder) const -> MovementDirection;
    static int sortDistanceAndDirectionBindersInAscendingOrder(void const * p1, void const* p2);
    static int sortDistanceAndDirectionBindersInDescendingOrder(void const* p1, void const* p2);

    Player const& player;

private:
    static constexpr QPointF initialPosition = {210, 210};
    static constexpr QPointF initialChasePoint = {210, 168};
    static constexpr QSize pixmapScaling = {26, 26};

    static auto getFrightenedSprites() -> SpriteMap<FrightState>;
    auto nextFrightState() const noexcept -> FrightState;
    auto nextSpriteIndex() const noexcept -> std::size_t;
    template <typename Key>
    static auto rescalePixmaps(SpriteMap<Key> spriteMap) -> SpriteMap<Key>;
    void setInitialPixmap();

    QTimer blinkingModeTimer, frightenedModeTimer;
    SpriteMap<MovementDirection> const regularSprites;
    SpriteMap<FrightState> const frightenedSprites;
    unsigned short blinkingInterval, movementTime, singleBlinkTime, runAwayTime;
    std::chrono::milliseconds const delayToLeaveHideout;
    FrightState frightState;
    std::size_t spriteIndex;
    bool frightened;

private slots:
    void allowToMove() override;
    void blink();
    void changeSprite();
    void disableRunawayState();
    void move() override;
    void releaseFromHideout();
};

#endif // ENEMY_H
