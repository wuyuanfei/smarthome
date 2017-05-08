#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QJsonObject>

class QTcpServer;
namespace Ui {
    class MainWindow;
}

class Message
{
public:
    unsigned char    type;       //消息类型
    char    funNum;     //功能号
    short   length;              //JSON数据长度
    QJsonObject json;           //JSON语句
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void analysis_str_form_cli(char *str,char *msg_funNum);


private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    quint16 blockSize;

    Message *msg;

    QString message;

    bool userInfoExist;
    bool temperatureExist;
    bool humidityExist;
    bool lightExist;
    bool triaxialXExist;

private slots:
    void    acceptConnection();
    void    sendMessage();
    void    readMessage();
    void displayError(QAbstractSocket::SocketError);
};

#endif // MAINWINDOW_H
