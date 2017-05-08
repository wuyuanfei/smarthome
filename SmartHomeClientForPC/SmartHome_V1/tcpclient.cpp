#include "tcpclient.h"

#include <QDebug>

tcpClient::tcpClient(QWidget *parent) : QWidget(parent)
{
    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readMessage()));

    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),

             this,SLOT(displayError(QAbstractSocket::SocketError)));

}

void tcpClient::newConnect()  //连接服务器
{
    blockSize = 0; //初始化其为0

    tcpSocket->abort(); //取消已有的连接

     tcpSocket->connectToHost(ui->hostLineEdit->text(),

                                 ui->portLineEdit->text().toInt());

        //连接到主机，这里从界面获取主机地址和端口号

}

void tcpClient::readMessage()  //接收数据
{
    QDataStream in(tcpSocket);

     in.setVersion(QDataStream::Qt_4_6);

     //设置数据流版本，这里要和服务器端相同

     if(blockSize==0) //如果是刚开始接收数据

     {

         //判断接收的数据是否有两字节，也就是文件的大小信息

         //如果有则保存到blockSize变量中，没有则返回，继续接收数据

         if(tcpSocket->bytesAvailable() < (int)sizeof(quint16)) return;

         in >> blockSize;

     }

     if(tcpSocket->bytesAvailable() < blockSize) return;

     //如果没有得到全部的数据，则返回，继续接收数据

     in >> message;

     //将接收到的数据存放到变量中

     ui->messageLabel->setText(message);

     //显示接收到的数据

}

void tcpClient::displayError(QAbstractSocket::SocketError)//显示错误
{
  qDebug() << tcpSocket->errorString(); //输出错误信息
}

