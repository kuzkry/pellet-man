#ifndef LIVESCOUNTER_H
#define LIVESCOUNTER_H

#include <QGraphicsTextItem>

class LivesCounter : public QGraphicsTextItem
{
public:
    LivesCounter();

    void decrease();
    auto getLives() const -> int
    {
        return lives;
    }

private:
    int lives;
};

#endif // LIVESCOUNTER_H
