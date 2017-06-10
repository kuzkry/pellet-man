#include "pellet.h"

Pellet::Pellet(int x, int y)
{
    setX(x); setY(y);
    setPixmap(QPixmap(":/sprites/sprites/pellet.png").scaled(15,15));
}
