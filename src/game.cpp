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
    initScene();
    initScore();
    initLifeCounter();
    deployNodes();
    deployRegularPellets();
    deploySuperPellets();
    createPlayer();
    createGhosts();
    initView();
}

void Game::run()
{
    player->init();
    for (Enemy* enemy : enemies)
        enemy->init();
    view.show();
}

void Game::initScene()
{
    // create the scene
    scene.setSceneRect(0, 0, gameWindow.width(), gameWindow.height());
    QImage const background(":/sprites/sprites/map.jpg");
    if (background.size() != gameWindow)
    {
        std::ostringstream errorMsg("size of provided background different than ");
        errorMsg << background.width() << 'x' << background.height();
        throw std::runtime_error(errorMsg.str());
    }
    scene.setBackgroundBrush(QBrush(background));
}

void Game::initScore()
{
    scene.addItem(&score);
}

void Game::initLifeCounter()
{
    lifeCounter.setPos(lifeCounter.x() + scene.width() - 62, lifeCounter.y());
    scene.addItem(&lifeCounter);
}

void Game::deployNodes()
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
        std::istringstream convertingStream(line.toStdString());
        qreal x, y;
        bool movements[4];
        convertingStream >> x >> y;
        for (unsigned short i = 0; i < 4; ++i)
            convertingStream >> movements[i];

        //then fill the vector and add to the scene
        nodes.push_back(Node{{x, y}, movements[0], movements[1], movements[2], movements[3]});
    }
    file.close();
}

void Game::deployRegularPellets()
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
        std::istringstream convertingStream(line.toStdString());
        qreal x, y;
        convertingStream >> x >> y;

        //then fill the vector and add to the scene
        regularPellets.push_back(new RegularPellet({x, y}));
        scene.addItem(regularPellets.back());
    }
    file.close();
}

void Game::deploySuperPellets()
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
        std::istringstream convertingStream(line.toStdString());
        qreal x, y;
        convertingStream >> x >> y;

        //then fill the vector and add to the scene
        superPellets.push_back(new SuperPellet({x, y}));
        scene.addItem(superPellets.back());
    }
    file.close();
}

void Game::createPlayer()
{
    // create the player
    player = new Player(nodes, score, lifeCounter, regularPellets, superPellets, [&]() { view.close(); }, enemies);
    // unfortunately player must get the reference to this game in order to call close function

    // make the player focusable and set it to be the current focus
    player->setFlag(QGraphicsItem::ItemIsFocusable);
    player->setFocus();
    // add the player to the scene
    scene.addItem(player);
}

void Game::createGhosts()
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

void Game::initView()
{
    view.setScene(&scene);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setFixedSize(gameWindow);
}
