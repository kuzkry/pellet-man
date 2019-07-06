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

    enum IncrementCause {REGULAR_PELLET, SUPER_PELLET, ENEMY_EATEN};

    void increase(IncrementCause);

public slots:
    void try_to_reset_multiplier();

private:
    static constexpr unsigned RegularPelletScore = 10;
    static constexpr unsigned SuperPelletScore = 50;

    void set_score();

    std::vector<Enemy*> const& enemies;
    unsigned score = 0;
    unsigned multiplier = 1;
};

#endif // SCORE_H
