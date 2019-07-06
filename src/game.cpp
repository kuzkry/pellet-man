#include "game.h"

#include "clyde.h"
#include "blinky.h"
#include "gamewindow.h"
#include "inky.h"
#include "pinky.h"
#include "player.h"
#include "regularpellet.h"
#include "superpellet.h"

#include <QBrush>
#include <QImage>
#include <QFile>
#include <QFont>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QtGlobal>

#include <cstdlib>
#include <ctime>
#include <sstream>
#include <stdexcept>

Game::Game() : score(enemies)
{
    init_scene();
    deploy_nodes();
    deploy_regular_pellets();
    deploy_super_pellets();
    create_player();
    create_ghosts();
    init_score();
    init_life_counter();
    init_view();
}

void Game::run()
{
    init_characters();
    view.show();
}

void Game::init_characters()
{
    player->init();
    for (Enemy* const enemy : enemies)
        enemy->init();
}

void Game::deinit_characters()
{
    player->deinit();
    for (Enemy* const enemy : enemies)
        enemy->deinit();
}

void Game::init_scene()
{
    // create the scene
    scene.setSceneRect(0, 0, GameWindow.width(), GameWindow.height());
    QImage const background(":/sprites/sprites/map.jpg");
    if (background.size() != GameWindow)
    {
        std::ostringstream error_msg("size of provided background different than ");
        error_msg << background.width() << 'x' << background.height();
        throw std::runtime_error(error_msg.str());
    }
    scene.setBackgroundBrush(QBrush(background));
}

void Game::deploy_nodes()
{
    //node values (position x, position y, upward, leftward, downward, rightward)
    QFile file(":/coordinates/coordinates/nodesCoordinates.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error("couldn't load nodes");

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.startsWith("//") || line.isEmpty())
            continue;
        /* so skip a line if it is either a commentary or an empty one */
        std::istringstream converting_stream(line.toStdString());
        qreal x, y;
        bool movements[4];
        converting_stream >> x >> y;
        for (unsigned short i = 0; i < 4; ++i)
            converting_stream >> movements[i];

        //then fill the vector and add to the scene
        nodes.push_back(Node{{x, y}, movements[0], movements[1], movements[2], movements[3]});
    }
}

void Game::deploy_regular_pellets()
{
    QFile file(":/coordinates/coordinates/regularPelletsCoordinates.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error("couldn't load regular pellets");

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.startsWith("//") || line.isEmpty())
            continue;
        /* so skip a line if it is either a commentary or an empty one */
        std::istringstream converting_stream(line.toStdString());
        qreal x, y;
        converting_stream >> x >> y;

        //then fill the vector and add to the scene
        regular_pellets.push_back(new RegularPellet({x, y}));
        scene.addItem(regular_pellets.back());
    }
}

void Game::deploy_super_pellets()
{
    QFile file(":/coordinates/coordinates/superPelletsCoordinates.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error("couldn't load super pellets");

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.startsWith("//") || line.isEmpty())
            continue;
        /* so skip a line if it is either a commentary or an empty one */
        std::istringstream converting_stream(line.toStdString());
        qreal x, y;
        converting_stream >> x >> y;

        //then fill the vector and add to the scene
        super_pellets.push_back(new SuperPellet({x, y}));
        scene.addItem(super_pellets.back());
    }
}

void Game::create_player()
{
    player = new Player(nodes, regular_pellets, super_pellets, enemies);
    // make the player focusable and set it to be the current focus
    player->setFlag(QGraphicsItem::ItemIsFocusable);
    player->setFocus();
    // add the player to the scene
    scene.addItem(player);

    QObject::connect(player, SIGNAL(interrupted()), &view, SLOT(close()));
    QObject::connect(player, SIGNAL(regular_pellet_eaten(RegularPellet*)), this, SLOT(handle_eating_regular_pellet(RegularPellet*)));
    QObject::connect(player, SIGNAL(regular_pellet_eaten(RegularPellet*)), this, SLOT(end_game_if_all_pellets_have_been_eaten()));
    QObject::connect(player, SIGNAL(super_pellet_eaten(SuperPellet*)), this, SLOT(handle_eating_super_pellet(SuperPellet*)));
    QObject::connect(player, SIGNAL(super_pellet_eaten(SuperPellet*)), this, SLOT(end_game_if_all_pellets_have_been_eaten()));
    QObject::connect(player, SIGNAL(enemy_hit(Enemy*)), this, SLOT(handle_enemy_hit(Enemy*)));
}

void Game::create_ghosts()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // creating ghosts
    auto const blinky = new Blinky(*player, nodes);
    auto const pinky = new Pinky(*player, nodes);
    auto const inky = new Inky(*player, nodes, *blinky);
    auto const clyde = new Clyde(*player, nodes);
    enemies = {blinky, pinky, inky, clyde};
    for (Enemy* const enemy : enemies)
        scene.addItem(enemy);
}

void Game::init_score()
{
    scene.addItem(&score);
    for (Enemy* const enemy : enemies)
        QObject::connect(enemy, SIGNAL(entered_chase_mode()), &score, SLOT(try_to_reset_multiplier()));
}

void Game::init_life_counter()
{
    life_counter.setPos(life_counter.x() + scene.width() - 62, life_counter.y());
    scene.addItem(&life_counter);
}

void Game::init_view()
{
    view.setScene(&scene);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setFixedSize(GameWindow);
}

void Game::set_game_end(EndGameReason const reason)
{
    deinit_characters();

    QGraphicsTextItem* text = nullptr;
    switch (reason)
    {
    case EndGameReason::VICTORY:
        text = new QGraphicsTextItem("YOU WIN!");
        break;
    case EndGameReason::DEFEAT:
        text = new QGraphicsTextItem("YOU LOSE!");
        break;
    }
    text->setPos(120, 210);
    text->setDefaultTextColor(Qt::red);
    text->setFont(QFont("times", 34));
    scene.addItem(text);

    QTimer::singleShot(DelayToCloseGame, &view, SLOT(close()));
}

void Game::end_game_if_all_pellets_have_been_eaten()
{
    if (regular_pellets.empty() && super_pellets.empty())
        set_game_end(EndGameReason::VICTORY);
}

void Game::handle_eating_regular_pellet(RegularPellet* const pellet)
{
    score.little_increase();

    auto const it = std::find(regular_pellets.cbegin(), regular_pellets.cend(), pellet);
    if (it == regular_pellets.cend())
        throw std::invalid_argument("pellet hasn't been found");

    scene.removeItem(pellet);
    delete pellet;
    regular_pellets.erase(it);
}

void Game::handle_eating_super_pellet(SuperPellet* const pellet)
{
    score.big_increase();

    auto const it = std::find(super_pellets.cbegin(), super_pellets.cend(), pellet);
    if (it == super_pellets.cend())
        throw std::invalid_argument("pellet hasn't been found");

    scene.removeItem(pellet);
    delete pellet;
    super_pellets.erase(it);

    for (Enemy* enemy : enemies)
        enemy->enable_runaway_state();
}

void Game::handle_enemy_hit(Enemy* const enemy)
{
    if (enemy->is_frightened())
    {
        score.huge_increase();
        enemy->init();
    }
    else
    {
        life_counter.decrease();
        if (life_counter.get_lives() != 0)
            init_characters();
        else
            set_game_end(EndGameReason::DEFEAT);
    }
}
