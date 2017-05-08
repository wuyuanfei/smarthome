#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>

//QTcpSocket *tcpSocket;      //定义为全局变量，多类同时使用

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void analysis_str_form_ser(char *str,char *funNum);

    void Login_OK();

    void Login_NO();

    void Register_OK();

    void Register_NO();

private slots:
    void on_LoginPushButton_clicked();

    void on_RegisterPushButton_clicked();

    //TCPClient通信
    void newConnect();//新连接
    void readMessage();//读取数据
    void displayError(QAbstractSocket::SocketError);
    void sendMessage();//发送数据

    void on_SettingPushButton_clicked();
    void on_ForgetPushButton_clicked();

private:
    Ui::Widget *ui;

    QString UserName;
    QString Passwd;

    quint16 blockSize;  //存放文件的大小信息

public:
};

#endif // WIDGET_H
