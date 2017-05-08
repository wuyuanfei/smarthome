#include "boxgroup.h"
#include "onebox.h"
#include <QList>
#include <QTimer>

BoxGroup::BoxGroup()
{
    setFlags(QGraphicsItem::ItemIsFocusable);
    timer = new QTimer(this);
    speed =500;

    connect(timer,SIGNAL(timeout()),this,SLOT(moveOneStep()));
}

/*
往BoxGroup里面添加4个OneBox，组成不同的图形
*/
void BoxGroup::createBoxGroup(BoxShape shape)
{

    static const QColor colorTable[7]={
        QColor(200,0,0,100),QColor(255,200,0,100),
        QColor(0,0,200,100),QColor(0,200,0,100),
        QColor(0,200,255,100),QColor(200,0,255,100),
        QColor(150,100,100,100)
    };

    currentShape = shape;

    if(currentShape ==  RandomShape)
    {
          currentShape = BoxShape(qrand()%7);
    }

    int shapeid = currentShape;

    QList<OneBox *> list;
    OneBox * item;
    for(int i=0;i<4;i++) {
        item = new OneBox(colorTable[shapeid]);
        list.append(item);
        this->addToGroup(item);//添加到组
    }

    switch (currentShape) {
    case IShape:
        list.at(0)->setPos(-30,-10);
        list.at(1)->setPos(-10,-10);
        list.at(2)->setPos(10,-10);
        list.at(3)->setPos(30,-10);
        break;
    case JShape:
        list.at(0)->setPos(10,-10);
        list.at(1)->setPos(10,10);
        list.at(2)->setPos(-10,30);
        list.at(3)->setPos(10,30);
        break;
    case LShape:
        list.at(0)->setPos(-10,-10);
        list.at(1)->setPos(-10,10);
        list.at(2)->setPos(-10,30);
        list.at(3)->setPos(10,30);
        break;
    case OShape:
        list.at(0)->setPos(-10,-10);
        list.at(1)->setPos(10,-10);
        list.at(2)->setPos(-10,10);
        list.at(3)->setPos(10,10);
        break;
    case SShape:
        list.at(0)->setPos(10,-10);
        list.at(1)->setPos(30,-10);
        list.at(2)->setPos(-10,10);
        list.at(3)->setPos(10,10);
        break;
    case TShape:
        list.at(0)->setPos(-10,-10);
        list.at(1)->setPos(10,-10);
        list.at(2)->setPos(30,-10);
        list.at(3)->setPos(10,10);
        break;
    case ZShape:
        list.at(0)->setPos(-10,-10);
        list.at(1)->setPos(10,-10);
        list.at(2)->setPos(10,10);
        list.at(3)->setPos(30,10);
        break;
    default:
        break;
    }
    if(isColliding())
    {
        emit gameover();
    }
}



QRectF BoxGroup::boundingRect() const
{
    qreal penWidth = 1;
       return QRectF(-40 -penWidth/2, -40 -penWidth/2,
                         80 +penWidth, 80 + penWidth);
}

void BoxGroup::moveOneStep()
{
    this->moveBy(0,20);
    if(isColliding())
    {
        moveBy(0,-20);
        stopMove();
        //将四个item从GroupBox移除，属于scene
        clearBoxGroup();
        emit fullRows();
    }
}

void BoxGroup::startMove(bool move)
{
    if(move)
        timer->start(speed);
}

void BoxGroup::stopMove()
{
    timer->stop();
}

/*
    遍历BoxGroup四个成员，只要有一个item与大于1个其它item碰撞了，
    表示BoxGroup 碰撞了
*/
bool BoxGroup::isColliding()
{
    QList<QGraphicsItem *> list = childItems();
    QGraphicsItem *item;
    foreach (item, list) {
        if(item->collidingItems().count() > 1)
            return true;
    }
    return false;
}

void BoxGroup::keyPressEvent(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        moveBy(-20,0);
        if(isColliding())
        {
          moveBy(20,0);
        }
        break;
    case Qt::Key_Up:
        /*QTransform().rotate(90)
            表示的是将图元当前 状态旋转90度需要结合的旋转矩阵
        */
        setTransform(QTransform().rotate(90),true);
        if(isColliding())
        {
             setTransform(QTransform().rotate(-90),true);
        }
        break;
    case Qt::Key_Right:
         moveBy(20,0);
         if(isColliding())
         {
           moveBy(-20,0);
         }
        break;
    case Qt::Key_Down:
        moveBy(0,20);
        while(!isColliding())
        {
             moveBy(0,20);
        }
        moveBy(0,-20);
        clearBoxGroup();
        emit fullRows();
        break;
    default:
        break;
    }
}

 void BoxGroup::clearBoxGroup(bool destroy)
 {
     //将四个item从BoxGroup移除
     QList<QGraphicsItem *> list = childItems();
     QGraphicsItem *item;
     foreach (item, list) {
         removeFromGroup(item);//将小item从group移除，并没有完全销毁
         if(destroy){
                OneBox * temp = (OneBox*)item;
                temp->deleteLater();
         }
     }
 }
