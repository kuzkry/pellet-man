#ifndef LIVESCOUNTER_H
#define LIVESCOUNTER_H

#include <QGraphicsTextItem>

class LivesCounter : public QGraphicsTextItem
{
public:
    LivesCounter();
    void decrease();
    int getLives() const
    {
        return lives;
    }
private:
    int lives;
};

#endif // LIVESCOUNTER_H
