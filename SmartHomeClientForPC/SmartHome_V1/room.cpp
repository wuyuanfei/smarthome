#include "room.h"
#include "ui_room.h"
#include "widget.h"
#include "message.h"
#include "smarthome.h"

#include <QDebug>
#include <QJsonDocument>
#include <QTimer>

extern QTcpSocket *tcpSocket;
extern QString IP;
extern int Port;

extern int LED1FrameValue;
extern int LED2FrameValue;
extern int FANFrameValue;
extern int DOORFrameValue;

room::room(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::room)
{
    ui->setupUi(this);

    //设置房间界面的label颜色
    QPalette pen;
    pen.setColor(QPalette::WindowText,Qt::white);
    //标题
    ui->TitleLabel->setPalette(pen);
    //时间
    ui->LED1Label->setPalette(pen);
    ui->LED2Label->setPalette(pen);
    ui->FanLabel->setPalette(pen);
    ui->DoorLabel->setPalette(pen);

    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/door/images/door/devmanger_bj.png").scaled( // 缩放背景图.
                             this->size(),
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation))); // 使用平滑的缩放方式
    this->setPalette(palette); // 至此, 已给widget加上了背景图.

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this, SLOT(LED1Changed()));
    connect(timer, SIGNAL(timeout()),this, SLOT(LED2Changed()));
    connect(timer, SIGNAL(timeout()),this, SLOT(FANChanged()));
    connect(timer, SIGNAL(timeout()),this, SLOT(DOORChanged()));
    timer->start(5);

    //newConnect();   //建立TCP连接
}

/**********************TCP通信***********************************/
void room::newConnect()
{
    blockSize = 0;// 初始化数据大小信息为0
    tcpSocket->abort();// 取消已有的连接
    tcpSocket->connectToHost(IP,Port);//与服务器连接

    if(tcpSocket->waitForConnected(1000))
    {
        qDebug()<<"连接成功！"<<endl;
    }
    else
        qDebug()<<"连接超时！"<<endl;
}

room::~room()
{
    delete ui;
}

int LED1 = 0;
int LED2 = 0;
int FAN = 0;
int DOOR = 0;
//0x08 打开关闭灯    1：关  0：开
//0x09 打开关闭风扇  1：关  0：开
//0x0a 打开关闭门    1：关  0：开
void room::on_LED1PushButton_clicked()
{
    LED1++;
    if(LED1%2){
        ui->LED1PushButton->setStyleSheet(tr("border-image: \
        url(:/door/images/door/switch_on.png);"));
        qDebug() << "-------------开灯-LED1----------------";
        send_request(0x08,0,0);
    }else{
        ui->LED1PushButton->setStyleSheet(tr("border-image: \
        url(:/door/images/door/switch_off.png);"));
        qDebug() << "-------------关灯-LED1---------------";
        send_request(0x08,1,0);
    }
}

void room::on_LED2PushButton_clicked()
{
    LED2++;
    if(LED2%2){
        ui->LED2PushButton->setStyleSheet(tr("border-image: \
        url(:/door/images/door/switch_on.png);"));
        qDebug() << "-------------开灯-LED2--------------";
        send_request(0x08,0,1);
    }else{
        ui->LED2PushButton->setStyleSheet(tr("border-image: \
        url(:/door/images/door/switch_off.png);"));
        qDebug() << "-------------关灯-LED2--------------";
        send_request(0x08,1,1);
    }
}

void room::on_FanPushButton_clicked()
{
    FAN++;
    if(FAN%2){
        ui->FanPushButton->setStyleSheet(tr("border-image: \
        url(:/door/images/door/switch_on.png);"));
        qDebug() << "----------------开风扇---------------";
        send_request(0x09,0,0);
    }else{
        ui->FanPushButton->setStyleSheet(tr("border-image: \
        url(:/door/images/door/switch_off.png);"));
        qDebug() << "----------------关风扇---------------";
        send_request(0x09,1,0);
    }
}

void room::on_DoorPushButton_clicked()
{
    DOOR++;
    if(DOOR%2){
        ui->DoorPushButton->setStyleSheet(tr("border-image: \
        url(:/door/images/door/switch_on.png);"));
        qDebug() << "----------------开门-----------------";
        send_request(0x0a,0,0);
    }else{
        ui->DoorPushButton->setStyleSheet(tr("border-image: \
        url(:/door/images/door/switch_off.png);"));
        qDebug() << "----------------关门-----------------";
        send_request(0x0a,1,0);
    }
}

void room::send_request(unsigned char msgType,int controlNum,int deviceNum)
{
    message RequestMsg;

    //填充登录信息类
    RequestMsg.type = 0xaa;
    RequestMsg.funNum = msgType;
    switch(msgType)
    {
    case 0x08:
        //qDebug() << "控制LED" ;
        RequestMsg.json.insert("userToken",QString("Qr5T4Y"));
        RequestMsg.json.insert("deviceNumber",deviceNum);
        RequestMsg.json.insert("deviceCode",controlNum);
        RequestMsg.json.insert("userName",QString("Qrt3T4"));
        break;

    case 0x09:
        //qDebug() << "控制风扇" ;
        RequestMsg.json.insert("userToken",QString("Qr5T4Y"));
        RequestMsg.json.insert("deviceNumber",deviceNum);
        RequestMsg.json.insert("deviceCode",controlNum);
        RequestMsg.json.insert("userName",QString("Qrt3T4"));
        break;

    case 0x0a:
        //qDebug() << "控制门" ;
        RequestMsg.json.insert("userToken",QString("Qr5T4Y"));
        RequestMsg.json.insert("deviceNumber",deviceNum);
        RequestMsg.json.insert("deviceCode",controlNum);
        RequestMsg.json.insert("userName",QString("Qrt3T4"));
        break;

     default:
        break;
    }

    //json对象转QString
    QJsonDocument document;
    document.setObject(RequestMsg.json);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    QString json_str(byte_array);

    //QString转char *
    QByteArray ba = json_str.toLatin1();
    char *str = ba.data();
    RequestMsg.length = json_str.length();

    //发送数据包到服务器端
    tcpSocket->write((char *)&RequestMsg,4);     //type + funnum + length
    tcpSocket->write(str,RequestMsg.length);          //json

    //发送数据成功后，显示提示
    qDebug() << "---------发送请求消息到服务器---------";
    qDebug("类  型：%#x",RequestMsg.type);
    qDebug("功能号：%#x",RequestMsg.funNum);
    qDebug("长  度：%d",RequestMsg.length);
    qDebug() << "json_str:" << str;
    qDebug() <<"请求信息发送成功" << endl;
}

void room::LED1Changed()
{
    if(LED1FrameValue%2){
        room::ui->LED1Frame->setStyleSheet(tr("border-image: \
        url(:/door/images/door/closeled_ico.png);"));
    }else{
        room::ui->LED1Frame->setStyleSheet(tr("border-image: \
        url(:/door/images/door/openled_ico.png);"));
    }
}

void room::LED2Changed()
{
    if(LED2FrameValue%2){
        room::ui->LED2Frame->setStyleSheet(tr("border-image: \
        url(:/door/images/door/closeled_ico.png);"));
    }else{
        room::ui->LED2Frame->setStyleSheet(tr("border-image: \
        url(:/door/images/door/openled_ico.png);"));
    }
}

void room::FANChanged()
{
    if(FANFrameValue%2){
        room::ui->FanFrame->setStyleSheet(tr("border-image: \
        url(:/door/images/door/closefan_ico.png);"));
    }else{
        room::ui->FanFrame->setStyleSheet(tr("border-image: \
        url(:/door/images/door/openfan_ico.png);"));
    }
}

void room::DOORChanged()
{
    if(DOORFrameValue%2){
        room::ui->DoorFrame->setStyleSheet(tr("border-image: \
        url(:/door/images/door/closedoor_ico.png);"));
    }else{
        room::ui->DoorFrame->setStyleSheet(tr("border-image: \
        url(:/door/images/door/opendoor_ico.png);"));
    }
}

void room::closeEvent(QCloseEvent *)
{
//    SmartHome *SmartHome_ui = new SmartHome;    //新建了一个类的页面
//    SmartHome_ui->show();                       //展示智能家居主界面

    this->close();
}


