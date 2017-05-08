#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QTcpSocket>

class tcpClient : public QWidget
{
    Q_OBJECT
public:
    explicit tcpClient(QWidget *parent = 0);

signals:

public slots:
    void newConnect();  //连接服务器
    void readMessage();  //接收数据
    void displayError(QAbstractSocket::SocketError);  //显示错误

private:
    QTcpSocket *tcpSocket;

    QString message;  //存放从服务器接收到的字符串

    quint16 blockSize;  //存放文件的大小信息
};

#endif // TCPCLIENT_H
