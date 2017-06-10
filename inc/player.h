#ifndef PLAYER_H
#define PLAYER_H

#include <QKeyEvent>
#include "score.h"
#include "livescounter.h"
#include "pellet.h"
#include "superpellet.h"
#include "game.h"
#include "character.h"
#include "enemy.h"

class Game;

class Player : public Character
{
    Q_OBJECT
public:
    Player(const std::vector<Node*> &nodes,
           std::unique_ptr<Score> &score,
           std::unique_ptr<LivesCounter> &livesCounter,
           std::vector<Pellet*> &pellets,
           std::vector<SuperPellet*> &superPellets,
           const Game &game,
           const std::vector<std::unique_ptr<Enemy>> &enemies);
    //last two are const to avoid inattentively usages of this (have to const_cast though)
    MovementDirection getCurrentDirection() const
    {
        return currentDirection;
    }
private:
    enum QuitReason{pressedEsc, lossOfLives, victory};

    void checkCollisionWithPelletsAndGhosts();
    void checkPositionWithRespectToNodes();
    void disable()
    {
        initialDelayTimer->stop();
        animationTimer->stop();
        movementTimer->stop();
    }
    void init();
    bool isAnyOfEnemiesFrightened() const;
    void keyPressEvent(QKeyEvent * event);
    void prepareToEndGame(QuitReason reason) const;
    void setMovement(const MovementDirection newDirection, bool movementPossibility = true);
    /* presumptive boolean value is true to spare the programmer's keyboard */

    MovementDirection pendingDirection;
    std::unique_ptr<Score> &score;
    std::unique_ptr<LivesCounter> &livesCounter;
    std::vector<Pellet*> &pellets;
    std::vector<SuperPellet*> &superPellets;
    std::unique_ptr<QTimer> animationTimer;
    const Game &game;
    const std::vector<std::unique_ptr<Enemy>> &enemies;
    unsigned short int initialDelay, movementTime, animationTime;
    bool moving;
private slots:
    void allowToMove();
    void chompingAnimation();
    void endGame() const;
    void move();
};

#endif // PLAYER_H
