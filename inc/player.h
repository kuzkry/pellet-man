#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

#include <chrono>

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

    auto getCurrentDirection() const noexcept -> MovementDirection
    {
        return currentDirection;
    }
    void init() override;

private:
    static constexpr QPointF initialPosition = {210, 347};
    static constexpr std::chrono::milliseconds movementTime{9};
    static constexpr std::chrono::milliseconds animationTime{100};
    static constexpr std::chrono::milliseconds initialDelay{1000};

    enum class QuitReason{PRESSED_ESC, DEFEAT, VICTORY};

    void setMovementInNode(Node const& node);
    void tryToSetOppositeMovement() noexcept;
    void deinit() override;
    void keyPressEvent(QKeyEvent* event) override;

    void checkCollisionWithPelletsAndGhosts();
    auto isAnyOfEnemiesFrightened() const -> bool;
    void prepareToEndGame(QuitReason reason) const;
    void setMovement(MovementDirection newDirection) noexcept;
    void stop() noexcept;

    MovementDirection pendingDirection;
    Score& score;
    LifeCounter& lifeCounter;
    std::vector<RegularPellet*>& regularPellets;
    std::vector<SuperPellet*>& superPellets;
    QTimer animationTimer;
    std::function<void()> quitCallback;
    std::vector<Enemy*> const& enemies;
    bool isMoving;

private slots:
    void allowToMove() override;
    void move() override;

    void chompingAnimation();
    void endGame() const;
};

#endif // PLAYER_H
