#include "game.h"

#include <QTextStream>
#include <sstream>
#include "blinky.h"
#include "pinky.h"
#include "inky.h"
#include "clyde.h"

Game::Game()
{
    createAndInitScene();
    createScore();
    createLifeCounter();
    deployNodes();
    deployRegularPellets();
    deploySuperPellets();
    createPlayer();
    createGhosts();
}

void Game::createAndInitScene()
{
    // create the scene
    scene.setSceneRect(0,0,450,480); // make the scene 450x480 instead of infinity by infinity (default)
    setBackgroundBrush(QBrush(QImage(":/sprites/sprites/map.jpg")));
    /* make the newly created scene the scene to visualize
     * (since Game is a QGraphicsView Widget, it can be used to visualize scenes) */
    setScene(&scene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(450,480);
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

inline void Game::createLifeCounter()
{
    lifeCounter.setPos(lifeCounter.x() + scene.width() - 62, lifeCounter.y());
    scene.addItem(&lifeCounter);
}

void Game::createPlayer()
{
    // create the player
    player = std::unique_ptr<Player>(new Player(nodes, score, lifeCounter, regularPellets, superPellets, *this, enemies));
    // unfortunately player must get the reference to this game in order to call close function

    // make the player focusable and set it to be the current focus
    player->setFlag(QGraphicsItem::ItemIsFocusable);
    player->setFocus();
    // add the player to the scene
    scene.addItem(player.get());
}

inline void Game::createScore()
{
    scene.addItem(&score);
}

void Game::deployNodes()
{
    //node values (position x, position y, upward, leftward, downward, rightward)
    QFile file(":/coordinates/coordinates/nodesCoordinates.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        class fileOpenFailure{}; //does not really matter but the game must load without this file
        throw fileOpenFailure();
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if(line.startsWith("//") || line.isEmpty()) continue;
        /* so skip a line if it is either a commentary or an empty one */
        std::istringstream convertingStream(line.toStdString());
        int x, y;
        bool movements[4];
        convertingStream >> x >> y;
        for(short unsigned int i = 0; i < 4; ++i) convertingStream >> movements[i];

        //then fill the vector and add to the scene
        nodes.push_back(new Node(x, y, movements[0], movements[1], movements[2], movements[3]));
        scene.addItem(nodes.back());
    }
    file.close();
}

void Game::deployRegularPellets()
{
    QFile file(":/coordinates/coordinates/regularPelletsCoordinates.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        class fileOpenFailure{}; //does not really matter but the game must not load without this file
        throw fileOpenFailure();
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if(line.startsWith("//") || line.isEmpty()) continue;
        /* so skip a line if it is either a commentary or an empty one */
        std::istringstream convertingStream(line.toStdString());
        qreal x, y;
        convertingStream >> x >> y;

        //then fill the vector and add to the scene
        regularPellets.push_back(new RegularPellet({x, y}));
        scene.addItem(regularPellets.back());
        show();
    }
    file.close();
}

void Game::deploySuperPellets()
{
    QFile file(":/coordinates/coordinates/superPelletsCoordinates.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        class fileOpenFailure{}; //does not really matter but the game must not load without this file
        throw fileOpenFailure();
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if(line.startsWith("//") || line.isEmpty()) continue;
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
