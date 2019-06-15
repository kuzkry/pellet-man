#include "score.h"
#include <QFont>

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
    if (multiplier == 1) score += 200;
    else if (multiplier == 2) score += 400;
    else if (multiplier == 3) score += 800;
    else if (multiplier == 4) score += 1600;
    setPlainText(QString("Score: ") + QString::number(score));
    ++multiplier;
}

void Score::little_increase()
{
    score += 10;
    setPlainText(QString("Score: ") + QString::number(score));
}

void Score::resetMultiplier()
{
    multiplier = 1;
}
