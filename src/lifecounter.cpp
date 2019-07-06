#include "lifecounter.h"

#include <QFont>
#include <QString>

LifeCounter::LifeCounter()
{
    lives = 3;

    setPlainText(QString("Lives: ") + QString::number(lives));
    setDefaultTextColor(Qt::white);
    setFont(QFont("times", 12));
}

void LifeCounter::decrease()
{
    --lives;
    QString const str = lives != 1 ? "Lives: " : "Life: ";
    setPlainText(str + QString::number(lives));
}
