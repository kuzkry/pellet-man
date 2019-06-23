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

    void init() override;

    auto getCurrentDirection() const noexcept -> MovementDirection;

private:
    static constexpr QPointF initialPosition = {210, 347};
    static constexpr std::chrono::milliseconds movementTime{9};
    static constexpr std::chrono::milliseconds animationTime{100};
    static constexpr std::chrono::milliseconds initialDelay{1000};

    enum class QuitReason{PRESSED_ESC, DEFEAT, VICTORY};

    void deinit() override;
    void keyPressEvent(QKeyEvent* event) override;

    void checkCollisionWithPelletsAndGhosts();
    auto isAnyOfEnemiesFrightened() const -> bool;
    auto getSprites() -> SpriteMap<MovementDirection>;
    void prepareToEndGame(QuitReason reason) const;
    void setMovement(MovementDirection newDirection) noexcept;
    void setMovementInNode(Node const& node);
    void stop() noexcept;
    void tryToSetOppositeMovement() noexcept;

    MovementDirection pendingDirection;
    Score& score;
    LifeCounter& lifeCounter;
    std::vector<RegularPellet*>& regularPellets;
    std::vector<SuperPellet*>& superPellets;
    std::function<void()> quitCallback;
    std::vector<Enemy*> const& enemies;
    bool isMoving;

private slots:
    void allowToMove() override;
    void changeSprite() override;
    void move() override;

    void endGame() const;
};

inline auto Player::getCurrentDirection() const noexcept -> MovementDirection
{
    return currentDirection;
}

#endif // PLAYER_H
