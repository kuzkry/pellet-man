#ifndef GAME_H
#define GAME_H

#include "lifecounter.h"
#include "player.h"
#include "score.h"

#include <QGraphicsScene>
#include <QGraphicsView>

#include <memory>
#include <vector>

class Enemy;
struct Node;
class RegularPellet;
class SuperPellet;

class Game : public QGraphicsView
{
public:
    Game();

private:
    void createAndInitScene();
    void createScore();
    void createLifeCounter();
    void deployNodes();
    void deployRegularPellets();
    void deploySuperPellets();
    void createPlayer();
    void createGhosts();
    QGraphicsScene scene;
    std::unique_ptr<Player> player;
    std::vector<Node*> nodes;
    std::vector<RegularPellet*> regularPellets;
    std::vector<SuperPellet*> superPellets;
    Score score;
    LifeCounter lifeCounter;
    std::vector<Enemy*> enemies;
};

#endif // GAME_H
