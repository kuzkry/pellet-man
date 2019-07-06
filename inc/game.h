#ifndef GAME_H
#define GAME_H

#include "lifecounter.h"
#include "node.h"
#include "score.h"

#include <QFile>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QObject>

#include <chrono>
#include <iterator>
#include <vector>

class Enemy;
class Player;
class RegularPellet;
class SuperPellet;

class Game : public QObject
{
    Q_OBJECT
public:
    Game();
    void run();

private:
    enum class EndGameReason{DEFEAT, VICTORY};
    static constexpr std::chrono::seconds DelayToCloseGame{3};

    void init_characters();
    void deinit_characters();
    void init_scene();
    void deploy_nodes();
    void deploy_regular_pellets();
    void deploy_super_pellets();
    template <typename T>
    void deploy_pellets(QFile& file, std::back_insert_iterator<T> it);
    void create_player();
    void create_ghosts();
    void init_score();
    void init_life_counter();
    void init_view();
    void set_game_end(EndGameReason);

    QGraphicsView view;
    QGraphicsScene scene;
    Player* player;
    std::vector<Node> nodes;
    std::vector<RegularPellet*> regular_pellets;
    std::vector<SuperPellet*> super_pellets;
    std::vector<Enemy*> enemies;
    Score score;
    LifeCounter life_counter;

private slots:
    void end_game_if_all_pellets_have_been_eaten();
    void handle_eating_regular_pellet(RegularPellet*);
    void handle_eating_super_pellet(SuperPellet*);
    void handle_enemy_hit(Enemy*);
};

#endif // GAME_H
