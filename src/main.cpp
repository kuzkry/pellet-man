#include <QApplication>
#include "game.h"

auto main(int argc, char* argv[]) -> int
{
    srand(time(NULL));
    QApplication a(argc, argv);

    Game game;
    game.show();

    return a.exec();
}
