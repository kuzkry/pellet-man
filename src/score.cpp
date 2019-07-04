#include "score.h"

#include <QFont>

#include <cmath>

Score::Score() : multiplier(0), score(0)
{
    // draw the text
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

void Score::reset_multiplier()
{
    multiplier = 1;
}
