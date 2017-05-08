#ifndef ROOM_H
#define ROOM_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class room;
}

class room : public QWidget
{
    Q_OBJECT

public:
    explicit room(QWidget *parent = 0);
    ~room();

    void send_request(unsigned char msgType,int controlNum,int deviceNum);

private slots:
    void on_LED1PushButton_clicked();

    void on_LED2PushButton_clicked();

    void on_FanPushButton_clicked();

    void on_DoorPushButton_clicked();

    void LED1Changed();
    void LED2Changed();
    void FANChanged();
    void DOORChanged();

    void newConnect();

private:
    Ui::room *ui;

    quint16 blockSize;  //存放文件的大小信息

protected:
    void closeEvent(QCloseEvent *);
};

#endif // ROOM_H
