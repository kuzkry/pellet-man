#ifndef LIFECOUNTER_H
#define LIFECOUNTER_H

#include <QGraphicsTextItem>

class LifeCounter : public QGraphicsTextItem
{
public:
    LifeCounter();

    void decrease();
    auto getLives() const noexcept -> unsigned short
    {
        return lives;
    }

private:
    unsigned short lives;
};

#endif // LIFECOUNTER_H
