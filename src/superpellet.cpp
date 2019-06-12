#include "superpellet.h"

SuperPellet::SuperPellet(QPointF const pos)
{
    setPos(pos);
    setPixmap(QPixmap(":/sprites/sprites/superpellet.png").scaled(16,16));
}
