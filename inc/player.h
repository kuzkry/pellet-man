#ifndef PLAYER_H
#define PLAYER_H

#include <QKeyEvent>
#include "score.h"
#include "livescounter.h"
#include "regularpellet.h"
#include "superpellet.h"
#include "character.h"
#include "enemy.h"

class Game;

class Player : public Character
{
    Q_OBJECT
public:
    Player(std::vector<Node*> const& nodes,
           Score& score,
           LivesCounter& livesCounter,
           std::vector<RegularPellet*>& regularPellets,
           std::vector<SuperPellet*>& superPellets,
           Game const& game,
           std::vector<Enemy*> const& enemies);
    //last two are const to avoid inattentively usages of this (have to const_cast though)

    MovementDirection getCurrentDirection() const
    {
        return currentDirection;
    }

private:
    enum QuitReason{PRESSED_ESC, DEFEAT, VICTORY};

    void checkPositionWithRespectToNodes() override;
    void disable() override;
    void init() override;
    void keyPressEvent(QKeyEvent* event) override;

    void checkCollisionWithPelletsAndGhosts();
    bool isAnyOfEnemiesFrightened() const;
    void prepareToEndGame(QuitReason reason) const;
    void setMovement(MovementDirection const newDirection, bool movementPossibility = true);
    /* presumptive boolean value is true to spare the programmer's keyboard */

    MovementDirection pendingDirection;
    Score& score;
    LivesCounter& livesCounter;
    std::vector<RegularPellet*>& regularPellets;
    std::vector<SuperPellet*>& superPellets;
    QTimer animationTimer;
    Game const& game;
    std::vector<Enemy*> const& enemies;
    unsigned short int initialDelay, movementTime, animationTime;
    bool moving;

private slots:
    void allowToMove() override;
    void move() override;

    void chompingAnimation();
    void endGame() const;
};

#endif // PLAYER_H
