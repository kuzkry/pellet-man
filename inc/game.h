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
    void init_scene();
    void init_score();
    void init_life_counter();
    void deploy_nodes();
    void deploy_regular_pellets();
    void deploy_super_pellets();
    void create_player();
    void create_ghosts();
    void init_view();

    QGraphicsView view;
    QGraphicsScene scene;
    Score score;
    LifeCounter life_counter;
    Player* player;
    std::vector<Node> nodes;
    std::vector<RegularPellet*> regular_pellets;
    std::vector<SuperPellet*> super_pellets;
    std::vector<Enemy*> enemies;
};

#endif // GAME_H
