#ifndef ENEMY_H
#define ENEMY_H

#include <map>
#include "character.h"

class Player;

class Enemy : public Character
{
public:
    Enemy(const Player &player, const std::vector<Node*> &nodes) : Character(nodes), player(player),
        movementTime(10), singleBlinkTime(20 * movementTime), blinkingInterval(2000), runAwayTime(8000) {}
    void checkPositionWithRespectToNodes();
    void disable()
    {
        initialDelayTimer.stop();
        movementTimer.stop();
        frightenedModeTimer.stop();
        blinkingModeTimer.stop();
    }
    void enableRunawayState();
    void init();
    bool isFrightened() const
    {
        return frightened;
    }
    virtual void setInitialPixmap() = 0;
protected:
    struct DistanceAndDirectionBinder
    {
        DistanceAndDirectionBinder(double distance, MovementDirection direction)
            : distance(distance), direction(direction) {}
        bool operator<(const DistanceAndDirectionBinder &ref) const
        {
            if(distance < ref.distance) return true;
            return false;
        }
        bool operator>(const DistanceAndDirectionBinder &ref) const
        {
            if(distance > ref.distance) return true;
            return false;
        }
        const float distance;
        const MovementDirection direction;
    };

    MovementDirection chooseMostSuitableTurnOption(std::map<MovementDirection, bool> &possibleMovements,
                                                   const DistanceAndDirectionBinder *binder) const;
    virtual MovementDirection makeTurnDecision(
            std::map<MovementDirection, bool> &possibleMovements, bool frightened) = 0;
    static int sortDistanceAndDirectionBindersInAscendingOrder(const void *p1, const void *p2);
    static int sortDistanceAndDirectionBindersInDescendingOrder(const void *p1, const void *p2);

    const Player &player;
    bool frightened, blinking;
    QTimer frightenedModeTimer;
    QTimer blinkingModeTimer;
    unsigned short int movementTime, singleBlinkTime, blinkingInterval, runAwayTime;
private:
    virtual void startInitialDelayTimer() = 0;
protected slots:
    virtual void blink() = 0;
    virtual void change() = 0;
    virtual void disableRunawayState() = 0;
    virtual void releaseFromGhostHouse() = 0;
};

/* anyway these virtual functions are going to be early bind and virtuality will not work on them
thus I put most of them in protected section */

#endif // ENEMY_H
