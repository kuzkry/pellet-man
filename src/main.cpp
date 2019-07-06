#include "game.h"

#include <QApplication>

auto main(int argc, char* argv[]) -> int
{
    QApplication a(argc, argv);

    Game game;
    game.run();

    return a.exec();
}
