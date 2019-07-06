#include "score.h"

#include "enemy.h"

#include <QFont>

#include <algorithm>
#include <cmath>

Score::Score(std::vector<Enemy*> const& enemies) : enemies(enemies)
{
    setPlainText(QString("Score: ") + QString::number(score)); // Score: 0
    setDefaultTextColor(Qt::white);
    setFont(QFont("times", 12));
}

void Score::big_increase()
{
    score += 50;
    setPlainText(QString("Score: ") + QString::number(score));
}

void Score::huge_increase()
{
    score += static_cast<unsigned>(200 * std::pow(2, ++multiplier - 1));
    setPlainText(QString("Score: ") + QString::number(score));
}

void Score::little_increase()
{
    score += 10;
    setPlainText(QString("Score: ") + QString::number(score));
}

void Score::try_to_reset_multiplier()
{
    if (std::none_of(enemies.cbegin(), enemies.cend(), [](Enemy* const enemy) { return enemy->is_frightened(); }))
        multiplier = 1;
}
