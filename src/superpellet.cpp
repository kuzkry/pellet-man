#include "superpellet.h"

SuperPellet::SuperPellet(int x, int y)
{
    setX(x); setY(y);
    setPixmap(QPixmap(":/sprites/sprites/superpellet.png").scaled(16,16));
}
