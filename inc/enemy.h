#ifndef ENEMY_H
#define ENEMY_H

#include "character.h"

#include <map>

class Player;

class Enemy : public Character
{
public:
    Enemy(Player const& player, std::vector<Node> const& nodes);

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

protected slots:
    virtual void blink() = 0;
    virtual void change() = 0;
    virtual void disableRunawayState() = 0;
    virtual void releaseFromGhostHouse() = 0;

private:
    virtual void startInitialDelayTimer() = 0;
};

/* anyway these virtual functions are going to be early bind and virtuality will not work on them
thus I put most of them in protected section */

#endif // ENEMY_H
