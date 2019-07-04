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
#include <QTextStream>
#include <QtGlobal>

#include <sstream>
#include <stdexcept>

Game::Game()
{
    init_scene();
    init_score();
    init_life_counter();
    deploy_nodes();
    deploy_regular_pellets();
    deploy_super_pellets();
    create_player();
    create_ghosts();
    init_view();
}

void Game::run()
{
    player->init();
    for (Enemy* enemy : enemies)
        enemy->init();
    view.show();
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

void Game::init_score()
{
    scene.addItem(&score);
}

void Game::init_life_counter()
{
    life_counter.setPos(life_counter.x() + scene.width() - 62, life_counter.y());
    scene.addItem(&life_counter);
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
    file.close();
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
    file.close();
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
    file.close();
}

void Game::create_player()
{
    // create the player
    player = new Player(nodes, score, life_counter, regular_pellets, super_pellets, [&]() { view.close(); }, enemies);
    // unfortunately player must get the reference to this game in order to call close function

    // make the player focusable and set it to be the current focus
    player->setFlag(QGraphicsItem::ItemIsFocusable);
    player->setFocus();
    // add the player to the scene
    scene.addItem(player);
}

void Game::create_ghosts()
{
    // creating ghosts
    enemies.emplace_back(new Blinky(*player, nodes));
    scene.addItem(enemies[0]);
    enemies.emplace_back(new Pinky(*player, nodes));
    scene.addItem(enemies[1]);
    enemies.emplace_back(new Inky(*player, nodes, dynamic_cast<Blinky&>(*enemies[0])));
    scene.addItem(enemies[2]);
    enemies.emplace_back(new Clyde(*player, nodes));
    scene.addItem(enemies[3]);
}

void Game::init_view()
{
    view.setScene(&scene);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setFixedSize(GameWindow);
}
