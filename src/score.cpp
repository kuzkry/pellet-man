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
    switch (multiplier) {
    case 1:
        score += 200;
        break;
    case 2:
        score += 400;
        break;
    case 3:
        score += 800;
        break;
    case 4:
        score += 1600;
        break;
    }
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
