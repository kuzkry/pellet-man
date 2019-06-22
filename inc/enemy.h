#ifndef ENEMY_H
#define ENEMY_H

#include "character.h"

#include <QPointF>

#include <chrono>
#include <map>

class Player;

class Enemy : public Character
{
    Q_OBJECT
public:
    Enemy(Player const& player, std::vector<Node> const& nodes, std::chrono::milliseconds delayToLeaveHideout);

    void checkPositionWithRespectToNodes() override;
    void disable() override;
    void init() override;
    virtual void setInitialPixmap() = 0;

    void enableRunawayState();
    auto isFrightened() const -> bool
    {
        return frightened;
    }

protected:
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
    bool frightened, blinking;
    QTimer frightenedModeTimer;
    QTimer blinkingModeTimer;
    unsigned short movementTime, singleBlinkTime, blinkingInterval, runAwayTime;
    std::chrono::milliseconds const delayToLeaveHideout;

protected slots:
    virtual void change() = 0;

private:
    static constexpr QPointF initialChasePoint = {210, 168};

    void startInitialDelayTimer();

private slots:
    void allowToMove() override;
    void blink();
    void disableRunawayState();
    void move() override;
    void releaseFromHideout();
};

/* anyway these virtual functions are going to be early bind and virtuality will not work on them
thus I put most of them in protected section */

#endif // ENEMY_H
