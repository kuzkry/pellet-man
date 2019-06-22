#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

class Enemy;
class LifeCounter;
class QKeyEvent;
class RegularPellet;
class Score;
class SuperPellet;

class Player : public Character
{
    Q_OBJECT
public:
    Player(std::vector<Node> const& nodes,
           Score& score,
           LifeCounter& lifeCounter,
           std::vector<RegularPellet*>& regularPellets,
           std::vector<SuperPellet*>& superPellets,
           std::function<void()> quitCallback,
           std::vector<Enemy*> const& enemies);
    //last two are const to avoid inattentively usages of this (have to const_cast though)

    auto getCurrentDirection() const -> MovementDirection
    {
        return currentDirection;
    }
    void init() override;

private:
    enum class QuitReason{PRESSED_ESC, DEFEAT, VICTORY};

    void checkPositionWithRespectToNodes() override;
    void deinit() override;
    void keyPressEvent(QKeyEvent* event) override;

    void checkCollisionWithPelletsAndGhosts();
    auto isAnyOfEnemiesFrightened() const -> bool;
    void prepareToEndGame(QuitReason reason) const;
    void setMovement(MovementDirection const newDirection, bool movementPossibility = true);
    /* presumptive boolean value is true to spare the programmer's keyboard */

    MovementDirection pendingDirection;
    Score& score;
    LifeCounter& lifeCounter;
    std::vector<RegularPellet*>& regularPellets;
    std::vector<SuperPellet*>& superPellets;
    QTimer animationTimer;
    std::function<void()> quitCallback;
    std::vector<Enemy*> const& enemies;
    unsigned short initialDelay, movementTime, animationTime;
    bool isMoving;

private slots:
    void allowToMove() override;
    void move() override;

    void chompingAnimation();
    void endGame() const;
};

#endif // PLAYER_H
