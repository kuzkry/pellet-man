#ifndef SCORE_H
#define SCORE_H

#include <QGraphicsTextItem>

#include <vector>

class Enemy;

class Score: public QGraphicsTextItem
{
    Q_OBJECT
public:
    Score(std::vector<Enemy*> const& enemies);

    void big_increase();
    void huge_increase();
    void little_increase();

public slots:
    void try_to_reset_multiplier();

private:
    std::vector<Enemy*> const& enemies;
    unsigned score = 0;
    unsigned multiplier = 1;
};

#endif // SCORE_H
