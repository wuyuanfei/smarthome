#include "smarthome.h"
#include "ui_smarthome.h"
#include "room.h"
#include "image.h"
#include "camera.h"
#include "message.h"
#include "widget.h"
#include "mainwindow.h"
#include "boxgameview.h"
#include "changepwd.h"
#include "videoplayer.h"

#include <QDebug>
#include <QTimer>
#include <QJsonDocument>
#include <QMessageBox>
#include <QMediaPlayer>

QString date_time_str;
QString msg_str = "    您的室内一切正常！";

extern QTcpSocket *tcpSocket;
extern QString TEMP;    //温度
extern QString HUMI;    //湿度
extern QString LIGHT;   //光照

extern int SANZHOU[3];  //三轴X,Y,Z


SmartHome::SmartHome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SmartHome)
{
    ui->setupUi(this);

    play_music();

    //设置登录主界面的label颜色
    QPalette pen;
    pen.setColor(QPalette::WindowText,Qt::white);

    //标题
    ui->TitleLabel->setPalette(pen);

    //时间
    ui->TimeLabel->setPalette(pen);

    ui->TempLabel->setPalette(pen);
    ui->HumidityLabel->setPalette(pen);
    ui->LightLabel->setPalette(pen);

    //视频监控，多媒体功能，设备管理，娱乐休闲
    ui->VideoPushButton->setPalette(pen);
    ui->MultiMediaPushButton->setPalette(pen);
    ui->DeviePushButton->setPalette(pen);
    ui->EntertainmentPushButton->setPalette(pen);

    if(!msg_str.isEmpty())
    {
        ui->MsgPushButton->setStyleSheet(tr("border-image: url(:/main/images/main/email_yes_ico.png);"));
    }

    //主界面背景设置
    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/main/images/main/home_bh.png").scaled( // 缩放背景图.
                             this->size(),
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation))); // 使用平滑的缩放方式
    this->setPalette(palette); // 至此, 已给widget加上了背景图.

    //实时显示时间
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this, SLOT(updateTime()));
    timer->start(1000);

    QTimer *timer_display = new QTimer(this);
    connect(timer_display, SIGNAL(timeout()),this, SLOT(showSensor()));
    timer_display->start(10);

    //请求获取温度,湿度,光照,三轴数据
//     timer_sensor1 = new QTimer(this);
//     timer_sensor2 = new QTimer(this);
//     timer_sensor3 = new QTimer(this);
//     timer_sensor4 = new QTimer(this);

//    connect(timer_sensor1, SIGNAL(timeout()),this, SLOT(send_temp_request()));
//    connect(timer_sensor2, SIGNAL(timeout()),this, SLOT(send_humi_request()));
//    connect(timer_sensor3, SIGNAL(timeout()),this, SLOT(send_light_request()));
//    connect(timer_sensor4, SIGNAL(timeout()),this, SLOT(send_sanzhou_request()));


//    timer_sensor1->start(5000);
//    timer_sensor2->start(7000);
//    timer_sensor3->start(10000);
//    timer_sensor4->start(3000);
}

void SmartHome::on_TempPushButton_clicked()
{
    send_temp_request();
}

void SmartHome::on_HumiPushButton_clicked()
{
    send_humi_request();
}

void SmartHome::on_LightPushButton_clicked()
{
    send_light_request();
}

void SmartHome::on_TraPushButton_clicked()
{
    send_sanzhou_request();
}

void SmartHome::sleep(int msec)//自定义Qt延时函数,单位毫秒
{
    QDateTime last = QDateTime::currentDateTime();
    QDateTime now;
    while (1)
    {
        now = QDateTime::currentDateTime();
        if (last.msecsTo(now) >= msec)
        {
            break;
        }
    }
}

/************************************************
 *
 * 发送温度，湿度，光照，三轴请求
 *
 * *********************************************/
void SmartHome::send_request(unsigned char funcNum)
{
    message request_msg;

    switch(funcNum)
    {
    case 0x04:
        qDebug() << "**********请求获取温度**********";
        request_msg.json.insert("userName",QString("Qrt3T4"));
        request_msg.json.insert("userToken",QString("Qr5T4Y"));
        request_msg.json.insert("deviceNumber",0);
    break;

    case 0x05:
        qDebug() << "**********请求获取湿度**********";
        request_msg.json.insert("userName",QString("Qrt3T4"));
        request_msg.json.insert("userToken",QString("Qr5T4Y"));
        request_msg.json.insert("deviceNumber",0);
    break;

    case 0x06:
        qDebug() << "**********请求获取光照**********";
        request_msg.json.insert("userName",QString("Qrt3T4"));
        request_msg.json.insert("userToken",QString("Qr5T4Y"));
        request_msg.json.insert("deviceNumber",0);
    break;

    case 0x07:
        qDebug() << "**********请求获取三轴数据**********";
        request_msg.json.insert("userName",QString("Qrt3T4"));
        request_msg.json.insert("userToken",QString("Qr5T4Y"));
        request_msg.json.insert("deviceNumber",0);
    break;

    default:
    break;
    }

    //封包
    request_msg.type = 0xaa;
    request_msg.funNum = funcNum;

    //json对象转QString
    QJsonDocument document;
    document.setObject(request_msg.json);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    QString json_str(byte_array);

    //QString转char *
    QByteArray ba = json_str.toLatin1();
    char *str = ba.data();
    request_msg.length = json_str.length();

    //发送数据包到服务器端
    tcpSocket->write((char *)&request_msg,4);     //type + funnum + length
    tcpSocket->write(str,request_msg.length);          //json

    //发送数据成功后，显示提示
    qDebug() << "---------发送请求消息到服务器---------";
    qDebug("类  型：%#x",request_msg.type);
    qDebug("功能号：%#x",request_msg.funNum);
    qDebug("长  度：%d",request_msg.length);
    qDebug() << "json_str:" << str;
    qDebug() <<"请求信息发送成功" << endl;
}


void SmartHome::updateTime()
{
    QDateTime time = QDateTime::currentDateTime();          //获取系统现在的时间
    date_time_str = time.toString("yyyy-MM-dd hh:mm:ss ddd"); //设置显示格式
    ui->TimeLabel->setText(date_time_str);                    //在标签上显示时间
}

void SmartHome::on_DeviePushButton_clicked()
{
    VideoPlayer *VideoPlayer_ui = new VideoPlayer;
    VideoPlayer_ui->show();               //展示娱乐界面
    qDebug() << "Welcome to VideoPlayer!" ;
}

void SmartHome::on_EntertainmentPushButton_clicked()
{
      BoxGameView *BoxGameView_ui = new BoxGameView;
      BoxGameView_ui->show();               //展示娱乐界面
      qDebug() << "Welcome to tetris game!" ;
}

void SmartHome::on_VideoPushButton_clicked()
{
    camera *camera_ui = new camera;    //新建了一个类的页面
    camera_ui->show();               //展示娱乐界面

    qDebug() << "Welcome to camera monitor!" ;
}

void SmartHome::on_MultiMediaPushButton_clicked()
{
    MainWindow *MainWindow_ui = new MainWindow;  //新建了一个类的页面
    MainWindow_ui->show();               //展示多媒体界面
    qDebug() << "Welcome to Multimedia!" ;
}

void SmartHome::on_AdminPushButton_clicked()
{
    ChangePwd  *ChangePwd_ui = new ChangePwd;
    ChangePwd_ui->show();
    qDebug() << "Welcome to changePasswd!" ;
}

void SmartHome::on_MainPushButton_clicked()
{
    //tcpSocket->abort();
//    timer_sensor1->stop();
//    timer_sensor2->stop();
//    timer_sensor3->stop();
//    timer_sensor4->stop();
    musicPlayer->stop();

    room *room_ui = new room;    //新建了一个类的页面
    room_ui->show();             //展示智能家居房间界面

    qDebug() << "Welcome to MyRoom!" ;
//    this->close();

}

void SmartHome::on_MsgPushButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("    您有新的消息！.");
    msgBox.setInformativeText(msg_str);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

//播放背景音乐
void SmartHome::play_music()
{
    /*************************播放音乐************************************/
    musicPlayer = new QMediaPlayer(this);
    musicPlayer->setMedia(QUrl::fromLocalFile("E:/qt_project/SmartHome_V1/sounds/Heaven.mp3"));
    musicPlayer->setVolume(70); //设置声音大小

    musicPlayer->play();
}

SmartHome::~SmartHome()
{
    delete ui;
}

//设置背景音乐，暂停与播放
int key = 0;
void SmartHome::on_pushButton_clicked()
{
    key++;
    if(key%2){
        ui->pushButton->setStyleSheet(tr("border-image: \
        url(:/main/images/main/speaker_mute_48px_563822_easyicon.net.png);"));
        musicPlayer->pause();
    }else{
        ui->pushButton->setStyleSheet(tr("border-image: \
        url(:/main/images/main/speaker_48px_563821_easyicon.net.png);"));
        musicPlayer->play();
    }
}

void SmartHome::showSensor()
{
    ui->TempLabel->setText("温度 " + TEMP +" ℃");
    ui->HumidityLabel->setText("湿度 " + HUMI+" %");
    ui->LightLabel->setText("光照 " + LIGHT+" Lx");
    ui->TraPushButton->move(SANZHOU[0],SANZHOU[1]);
}

void SmartHome::send_temp_request()
{
    send_request(0x04);
}

void SmartHome::send_humi_request()
{
    send_request(0x05);
}

void SmartHome::send_light_request()
{
    send_request(0x06);
}

void SmartHome::send_sanzhou_request()
{
    send_request(0x07);
}

void SmartHome::closeEvent(QCloseEvent *)
{
    tcpSocket->abort();
//    timer_sensor1->stop();
//    timer_sensor2->stop();
//    timer_sensor3->stop();
//    timer_sensor4->stop();
    musicPlayer->stop();
    this->close();
}
