#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>
#include <memory>
#include <vector>
#include "player.h"
#include "node.h"
#include "score.h"
#include "lifecounter.h"
#include "regularpellet.h"
#include "superpellet.h"
#include "enemy.h"

class Player;
class Enemy;

class Game : public QGraphicsView
{
public:
    Game();
private:
    void createAndInitScene();
    void createGhosts();
    void createLifeCounter();
    void createPlayer();
    void createScore();
    void deployNodes();
    void deployRegularPellets();
    void deploySuperPellets();
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
