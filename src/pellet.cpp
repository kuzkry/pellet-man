#include "pellet.h"

Pellet::Pellet(QPointF const pos)
{
    setPos(pos);
    setPixmap(QPixmap(":/sprites/sprites/pellet.png").scaled(15,15));
}
