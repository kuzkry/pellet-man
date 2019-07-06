#include "score.h"

#include "enemy.h"

#include <QFont>

#include <algorithm>
#include <cmath>

Score::Score(std::vector<Enemy*> const& enemies) : enemies(enemies)
{
    setDefaultTextColor(Qt::white);
    setFont(QFont("times", 12));
    set_score();
}

void Score::big_increase()
{
    score += 50;
    set_score();
}

void Score::huge_increase()
{
    score += static_cast<unsigned>(200 * std::pow(2, ++multiplier - 1));
    set_score();
}

void Score::little_increase()
{
    score += 10;
    set_score();
}

void Score::try_to_reset_multiplier()
{
    if (std::none_of(enemies.cbegin(), enemies.cend(), [](Enemy* const enemy) { return enemy->is_frightened(); }))
        multiplier = 1;
}

void Score::set_score()
{
    setPlainText("Score: " + QString::number(score));
}
