#ifndef BOXGAMEVIEW_H
#define BOXGAMEVIEW_H

#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QMediaPlayer>
#include "boxgroup.h"


class BoxGameView : public QGraphicsView
{
     Q_OBJECT
public:
    BoxGameView();

    void moveBoxs();

     void closeEvent(QCloseEvent *event);

private:
    QGraphicsLineItem *leftline;
    QGraphicsLineItem *topline;
    QGraphicsLineItem *rightline;
    QGraphicsLineItem *bottomline;

    QGraphicsTextItem * scoreText;
    QGraphicsTextItem * gameoverText;
    QGraphicsTextItem *label;
    BoxGroup * currentGroup;
    BoxGroup * nextGroup;
    int level;
    QList<int> rows;//记录所有满行的行号
    int score;

    QMediaPlayer *musicPlayer;

public slots:
     void clearFullRows();
     void gameoverSlot();

};

#endif // BOXGAMEVIEW_H
