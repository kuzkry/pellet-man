#ifndef PELLET_H
#define PELLET_H

#include <QGraphicsPixmapItem>
#include <QPointF>

class Pellet : public QGraphicsPixmapItem
{
public:
    Pellet(QPointF pos);
};

#endif // PELLET_H
