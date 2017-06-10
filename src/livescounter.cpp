#include "livescounter.h"
#include "game.h"
#include "player.h"
#include <QFont>
#include <QGraphicsScene>
#include <QTimer>

LivesCounter::LivesCounter()
{
    // initialize lives to 0
    lives = 3;

    // draw the text
    setPlainText(QString("Lives: ") + QString::number(lives));
    setDefaultTextColor(Qt::white);
    setFont(QFont("times",12));
}

void LivesCounter::decrease()
{
    --lives;
    setPlainText(QString("Lives: ") + QString::number(lives));
}
