#ifndef LIFECOUNTER_H
#define LIFECOUNTER_H

#include <QGraphicsTextItem>

class LifeCounter : public QGraphicsTextItem
{
public:
    LifeCounter();

    void decrease();
    auto getLives() const -> int
    {
        return lives;
    }

private:
    int lives;
};

#endif // LIFECOUNTER_H
