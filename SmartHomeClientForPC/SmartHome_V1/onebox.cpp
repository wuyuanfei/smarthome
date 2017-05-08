#include "onebox.h"

OneBox::OneBox(QColor color)
{
    currentColor = color;
}


void OneBox::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    //底层绘制灰色图片
    //上面再绘制不同颜色层
    painter->drawPixmap(-10,-10,20,20,QPixmap(":/games/images/games/box.gif"));
    currentColor.setAlpha(40);
    painter->setBrush(currentColor);
    painter->setPen(currentColor);
    painter->drawRect(QRect(-10,-10,20,20));

}

QPainterPath OneBox::shape() const
{
    QPainterPath path;
    //将shape边框设置小于真实图元大小，
    //碰撞检测函数默认调用的shape检测碰撞
    //紧挨着的两个图元直接不会被检查出碰撞
    path.addRect(-9.5,-9.5,19,19);
    return path;
}

QRectF OneBox::boundingRect() const
{
    qreal penWidth = 1;
       return QRectF(-10 -penWidth/2, -10 -penWidth/2,
                         20 +penWidth, 20 + penWidth);
}

