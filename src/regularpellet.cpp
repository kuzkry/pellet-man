#include "regularpellet.h"

RegularPellet::RegularPellet(QPointF const pos)
    : Pellet(pos, QPixmap(":/sprites/sprites/pellet.png").scaled(15, 15)) {}
