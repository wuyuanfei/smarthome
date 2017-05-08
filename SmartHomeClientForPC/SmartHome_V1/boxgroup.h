#ifndef BOXGROUP_H
#define BOXGROUP_H

#include <QGraphicsItemGroup>
#include <QObject>
#include <QKeyEvent>

class BoxGroup :public QObject,public QGraphicsItemGroup
{
    Q_OBJECT
public:
    enum BoxShape{IShape,JShape,LShape,OShape,SShape,TShape,ZShape,RandomShape};
    BoxShape currentShape;
    BoxGroup();
    QRectF boundingRect() const;
    void startMove(bool);
    void stopMove();
    void createBoxGroup(BoxShape shape = RandomShape);
    bool isColliding();
    void clearBoxGroup(bool destroy = false);

protected:
    void keyPressEvent(QKeyEvent * event);


private:

    QTimer *timer;
    int speed;

signals:
    void fullRows();
    void gameover();

public slots:
    void moveOneStep();
};

#endif // BOXGROUP_H
