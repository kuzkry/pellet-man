#include "lifecounter.h"
#include "game.h"
#include "player.h"
#include <QFont>
#include <QGraphicsScene>
#include <QTimer>

LifeCounter::LifeCounter()
{
    // initialize lives to 0
    lives = 3;

    // draw the text
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
