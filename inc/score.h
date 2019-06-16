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
    void resetMultiplier();

private:
    unsigned short int multiplier;
    unsigned short int score;
};

#endif // SCORE_H
