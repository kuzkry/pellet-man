#include "game.h"

#include <QApplication>

#include <cstdlib>
#include <ctime>

auto main(int argc, char* argv[]) -> int
{
    std::srand(std::time(nullptr));
    QApplication a(argc, argv);

    Game game;
    game.show();

    return a.exec();
}
