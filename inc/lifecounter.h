#ifndef LIFECOUNTER_H
#define LIFECOUNTER_H

#include <QGraphicsTextItem>

class LifeCounter : public QGraphicsTextItem
{
public:
    LifeCounter();

    void decrease();
    auto get_lives() const noexcept -> unsigned short;

private:
    unsigned short lives;
};

inline auto LifeCounter::get_lives() const noexcept -> unsigned short
{
    return lives;
}

#endif // LIFECOUNTER_H
