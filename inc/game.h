#ifndef GAME_H
#define GAME_H

#include "lifecounter.h"
#include "node.h"
#include "score.h"

#include <QGraphicsScene>
#include <QGraphicsView>

#include <chrono>
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
    enum class EndGameReason{DEFEAT, VICTORY};
    static constexpr std::chrono::seconds DelayToCloseGame{3};

    void init_scene();
    void init_score();
    void init_life_counter();
    void deploy_nodes();
    void deploy_regular_pellets();
    void deploy_super_pellets();
    void create_player();
    void create_ghosts();
    void init_view();
    void set_game_end(EndGameReason);

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
