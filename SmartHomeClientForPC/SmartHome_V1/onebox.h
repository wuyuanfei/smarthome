#ifndef ONEBOX_H
#define ONEBOX_H

#include <QGraphicsObject>
#include <QColor>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>


class OneBox : public QGraphicsObject
{
public:
    OneBox(QColor);

public:
    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    QPainterPath shape() const;

private:
    QColor currentColor;
};

#endif // ONEBOX_H
