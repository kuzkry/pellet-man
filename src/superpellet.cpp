#include "superpellet.h"

SuperPellet::SuperPellet(QPointF const pos)
    : Pellet(pos, QPixmap(":/sprites/sprites/superpellet.png").scaled(16,16)) {}
