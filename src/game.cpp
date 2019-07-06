#include "game.h"

#include "clyde.h"
#include "blinky.h"
#include "gameconstants.h"
#include "inky.h"
#include "pinky.h"
#include "player.h"
#include "regularpellet.h"
#include "superpellet.h"

#include <QBrush>
#include <QImage>
#include <QFont>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QtGlobal>

#include <cstdlib>
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <type_traits>

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
    scene.setSceneRect(0, 0, GameWindow.width(), GameWindow.height());
    QImage const background(":/sprites/sprites/map.jpg");
    if (background.size() != GameWindow)
    {
        std::ostringstream error_msg("size of provided background different than ", std::ios_base::ate);
        error_msg << background.width() << 'x' << background.height();
        throw std::runtime_error(error_msg.str());
    }
    scene.setBackgroundBrush(QBrush(background));
}

void Game::deploy_nodes()
{
    QFile file(":/coordinates/coordinates/nodesCoordinates.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error("couldn't load nodes");

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.startsWith("//") || line.isEmpty())
            continue;

        std::istringstream converting_stream(line.toStdString());
        qreal x, y;
        bool movements[4];
        converting_stream >> x >> y;
        for (unsigned short i = 0; i < 4; ++i)
            converting_stream >> movements[i];

        // then fill the vector and add to the scene
        nodes.push_back(Node{{x, y}, movements[0], movements[1], movements[2], movements[3]});
    }
}

void Game::deploy_regular_pellets()
{
    QFile file(":/coordinates/coordinates/regularPelletsCoordinates.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error("couldn't load regular pellets");

    deploy_pellets(file, std::back_inserter(regular_pellets));

    if (regular_pellets.size() != RegularPelletCount)
    {
        std::ostringstream error_msg("expected number of regular pellets ", std::ios_base::ate);
        error_msg << '[' << RegularPelletCount << ']'
                  << " differs from provided "
                  << '[' << regular_pellets.size() << ']';
        throw std::runtime_error(error_msg.str());
    }
}

void Game::deploy_super_pellets()
{
    QFile file(":/coordinates/coordinates/superPelletsCoordinates.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error("couldn't load super pellets");

    deploy_pellets(file, std::back_inserter(super_pellets));

    if (super_pellets.size() != SuperPelletCount)
    {
        std::ostringstream error_msg("expected number of super pellets ", std::ios_base::ate);
        error_msg << '[' << SuperPelletCount << ']'
                  << " differs from provided "
                  << '[' << super_pellets.size() << ']';
        throw std::runtime_error(error_msg.str());
    }
}

template <typename T>
void Game::deploy_pellets(QFile& file, std::back_insert_iterator<T> it)
{
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString const line = in.readLine();
        if (line.startsWith("//") || line.isEmpty())
            continue;

        std::istringstream converting_stream(line.toStdString());
        qreal x, y;
        converting_stream >> x >> y;

        // then fill the vector and add to the scene
        using pellet_type = std::remove_pointer_t<typename decltype(it)::container_type::value_type>;
        pellet_type* const pellet = new pellet_type({x, y});
        it = pellet;
        scene.addItem(pellet);
    }
}

void Game::create_player()
{
    player = new Player(nodes, regular_pellets, super_pellets, enemies);
    player->setFlag(QGraphicsItem::ItemIsFocusable);
    player->setFocus(); // Player can now receive keyboard input
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

    auto const blinky = new Blinky(*player, nodes);
    auto const pinky = new Pinky(*player, nodes);
    auto const inky = new Inky(*player, nodes, *blinky);
    auto const clyde = new Clyde(*player, nodes);
    enemies = {blinky, pinky, inky, clyde};

    if (enemies.size() != EnemyCount)
    {
        std::ostringstream error_msg;
        error_msg << enemies.size()
                  << " enemies has been created, expected quantity is "
                  << EnemyCount;
        throw std::logic_error(error_msg.str());
    }

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
    score.increase(Score::IncrementCause::REGULAR_PELLET);

    auto const it = std::find(regular_pellets.cbegin(), regular_pellets.cend(), pellet);
    if (it == regular_pellets.cend())
        throw std::invalid_argument("pellet hasn't been found");

    scene.removeItem(pellet);
    delete pellet;
    regular_pellets.erase(it);
}

void Game::handle_eating_super_pellet(SuperPellet* const pellet)
{
    score.increase(Score::IncrementCause::SUPER_PELLET);

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
        score.increase(Score::IncrementCause::ENEMY_EATEN);
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
