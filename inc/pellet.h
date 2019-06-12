#ifndef PELLET_H
#define PELLET_H

#include <QGraphicsPixmapItem>
#include <QPointF>
#include <QPixmap>

class Pellet : public QGraphicsPixmapItem
{
protected:
    Pellet(QPointF pos, QPixmap pixmap);
};

#endif // PELLET_H
