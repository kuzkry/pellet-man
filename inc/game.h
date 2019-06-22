#ifndef GAME_H
#define GAME_H

#include "lifecounter.h"
#include "node.h"
#include "score.h"

#include <QGraphicsScene>
#include <QGraphicsView>

#include <vector>

class Enemy;
class Player;
class RegularPellet;
class SuperPellet;

class Game
{
public:
    Game();
    void run();

private:
    void createAndInitScene();
    void createScore();
    void createLifeCounter();
    void deployNodes();
    void deployRegularPellets();
    void deploySuperPellets();
    void createPlayer();
    void createGhosts();
    QGraphicsView view;
    QGraphicsScene scene;
    Player* player;
    std::vector<Node> nodes;
    std::vector<RegularPellet*> regularPellets;
    std::vector<SuperPellet*> superPellets;
    Score score;
    LifeCounter lifeCounter;
    std::vector<Enemy*> enemies;
};

#endif // GAME_H
