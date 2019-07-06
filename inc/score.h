#ifndef SCORE_H
#define SCORE_H

#include <QGraphicsTextItem>

class Score: public QGraphicsTextItem
{
public:
    Score();

    void big_increase();
    void huge_increase();
    void little_increase();
    void reset_multiplier();

private:
    unsigned score;
    unsigned short multiplier;
};

#endif // SCORE_H
