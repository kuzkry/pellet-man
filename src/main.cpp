#include <QApplication>
#include "game.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));
    QApplication a(argc, argv);

    Game *game = new Game(); // for some reason it cannot be wrapped in any smart pointer
    game->show();

    return a.exec();
}
