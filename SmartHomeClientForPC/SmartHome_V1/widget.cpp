#include "widget.h"
#include "ui_widget.h"
#include "smarthome.h"
#include "register.h"
#include "tcpclient.h"
#include "message.h"
#include "setting.h"
#include "register.h"
#include "forgetpas.h"
#include "room.h"

#include <QMessageBox>
#include <QDebug>
#include <QDialog>
#include <QJsonDocument>
#include <QTimer>
#include <QtEndian>

//外部引用
extern QString IP;
extern int Port;

extern QString chaPasUserToken;   //修改密码的userToken

//全局变量区
QTcpSocket *tcpSocket;
QString TEMP = "0";    //温度
QString HUMI = "0";    //湿度
QString LIGHT = "0";   //光照
int SANZHOU[3] = {0};         //三轴

int LED1FrameValue = 1;
int LED2FrameValue = 1;
int FANFrameValue = 1;
int DOORFrameValue = 1;
int VideoFrameValue = 1;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //设置登录主界面的label颜色
    QPalette pen;
    pen.setColor(QPalette::WindowText,Qt::white);
    ui->TitleLabel->setPalette(pen);
    ui->ProLabel->setPalette(pen);
    ui->CopyrightLabel->setPalette(pen);
    ui->groupBox->setPalette(pen);

    //用户名以及密码
    ui->UserNameLabel->setPalette(pen);
    ui->PasswdLabel->setPalette(pen);
    ui->ForgetPushButton->setPalette(pen);

    ui->PasswdLineEdit->setEchoMode(QLineEdit::Password);//设置密码编辑行为暗码

    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/login/images/login/login_bj.jpg").scaled( // 缩放背景图.
                             this->size(),
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation))); // 使用平滑的缩放方式
    this->setPalette(palette); // 至此, 已给widget加上了背景图.

    //TCP通信设置
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage()));//当有消息接受时会触发接收
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));

    ui->UserNameLineEdit->setText("admin");
    ui->PasswdLineEdit->setText("admin");
}

Widget::~Widget()
{
    delete ui;
}

/**************************************
 * 登录系统
 * ***********************************/
void Widget::on_LoginPushButton_clicked()
{
    newConnect();   //建立TCP连接

    sendMessage();  //发送用户名和密码到服务器
}


/**************************************
 * 注册用户
 * ************************************/
void Widget::on_RegisterPushButton_clicked()
{
    newConnect();   //建立TCP连接

    Register *Register_ui = new Register;   //新建了一个类的页面
    Register_ui->show();                    //展示添加用户界面
}

/**********************TCP通信***********************************/
void Widget::newConnect()
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

/*****************************************************
 *
 * 发送消息到服务器
 *
 * ***************************************************/
void Widget::sendMessage()
{
    UserName = ui->UserNameLineEdit->text();
    Passwd = ui->PasswdLineEdit->text();

    message LoginMsg;
    //填充登录信息类
    LoginMsg.type = 0xaa;
    LoginMsg.funNum = 0x01;
    LoginMsg.json.insert("password",Passwd);
    LoginMsg.json.insert("userName",UserName);

    //json对象转QString
    QJsonDocument document;
    document.setObject(LoginMsg.json);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    QString json_str(byte_array);

    //QString转char *
    QByteArray ba = json_str.toLatin1();
    char *str = ba.data();
    LoginMsg.length = json_str.length();

    //发送数据包到服务器端
    tcpSocket->write((char *)&LoginMsg,4);     //type + funnum + length
    tcpSocket->write(str,LoginMsg.length);          //json

    //发送数据成功后，显示提示
    qDebug() << "---------发送消息到服务器---------";
    qDebug("类  型：%#x",LoginMsg.type);
    qDebug("功能号：%#x",LoginMsg.funNum);
    qDebug("长  度：%d",LoginMsg.length);
    qDebug() << "json_str:" << str;
    qDebug() <<"登录信息发送成功" << endl;
}

/*****************************************************
 *
 * 接收来自服务器的消息
 *
 * ***************************************************/
void Widget::readMessage()
{
    message Recvmsg;

    tcpSocket->read((char *)&Recvmsg.type,4);    //sizeof(msg.type)+sizeof(msg.funNum)+sizeof(msg.length));
    char str[Recvmsg.length + 1] = {0};      //字符串末尾的'\0'
    tcpSocket->read(str, Recvmsg.length);
    //while((sizeof(Recvmsg) + sizeof(str)) < (4+Recvmsg.length));

    //str[strlen(str) + 1] = '\0';

//    unsigned char msg_type = qToLittleEndian(Recvmsg.type);
//    unsigned char msg_funNum = qToLittleEndian(Recvmsg.funNum);
//    unsigned char msg_length = qToLittleEndian(Recvmsg.length);

    // 显示接收到的数据
    qDebug() << "---------接收来自服务器的消息---------";
    qDebug("类  型：%#x",Recvmsg.type);
    qDebug("功能号：%#x",Recvmsg.funNum);
    qDebug("长  度：%d",Recvmsg.length);
    qDebug() << "json_str:" << str;
    qDebug() <<"------------接收信息成功-----------" << endl;

    if(Recvmsg.type == 0xff)
    {
        switch(Recvmsg.funNum)
        {
        case 0x00:
            qDebug() << "**********用户注册**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;

        case 0x01:
            qDebug() << "**********用户登陆**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;

        case 0x02:
            qDebug() << "**********忘记密码**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;

        case 0x21:
            qDebug() << "**********发送原密码**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;

        case 0x03:
            qDebug() << "**********修改密码**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;

        case 0x04:
            qDebug() << "**********请求获取温度**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;

        case 0x05:
            qDebug() << "**********请求获取湿度**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;

        case 0x06:
            qDebug() << "**********请求获取光照**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;

        case 0x07:
            qDebug() << "**********请求获取三轴数据**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;

        case 0x08:
            qDebug() << "**********打开/关闭灯**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;

        case 0x09:
            qDebug() << "**********打开/关闭风扇**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;

        case 0x0a:
            qDebug() << "**********打开/关闭门**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;

        case 0x0b:
            qDebug() << "**********得到视频文件**********";
            analysis_str_form_ser(str,&Recvmsg.funNum);
        break;  

        default:
            break;
        }
    }else{
        qDebug() << "消息类型不正确！";
    }
}

/*****************************************************
 *
 * 服务器端json字符串消息解析
 *
 * ***************************************************/
void  Widget::analysis_str_form_ser(char *str,char *funNum)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(str, &json_error);

    //1.解析来自服务器的状态码
    QString userName; 		//用户名
    QString oldPassword; 	//旧密码
    QString newPassword; 	//新密码
    QString passWord; 		//服务器发送原密码
    QString phoneNumber; 	//手机号
    QString userToken; 		//用户身份识别码
    QString randomCode; 	//验证码（6 位大小写字母和数字组成）
    int stateCode; 			//状态值（int）
    double temperature = 0; 		//温度
    int humidity = 0; 			//湿度
    int triaxialX = 0; 			//三轴 x
    int triaxialY = 0; 			//三轴 y
    int triaxialZ = 0; 			//三轴 z
    int light = 0 ; 				//光照
    int deviceNumber; 		//设备号
    int deviceCode; 		//设备操作码
    int deviceState; 		//设备状态码
    //int videoList; 			//获取视频文件列表

    qDebug() << "-----APP端Json解析-----";
    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QJsonObject obj = parse_doucment.object();

            if(obj.contains("stateCode"))
            {
                QJsonValue stateCode_value = obj.take("stateCode");
                if(stateCode_value.isDouble())
                {
                    stateCode = stateCode_value.toVariant().toInt();
                    qDebug("stateCode:%d",stateCode);
                }
            }

            if(obj.contains("userToken"))
            {
                 QJsonValue userToken_value = obj.take("userToken");
                if(userToken_value.isString())
                {
                    userToken = userToken_value.toString();
                    qDebug() << "userToken:" << userToken;
                }
            }

            if(obj.contains("temperature"))
            {
                 QJsonValue temperature_value = obj.take("temperature");
                if(temperature_value.isDouble())
                {
                    //temperature = temperature_value.toVariant().toInt();
                    temperature = temperature_value.toDouble();
                    qDebug() << "temperature:" << temperature << "℃";
                }
            }

            if(obj.contains("humidity"))
            {
                 QJsonValue humidity_value = obj.take("humidity");
                if(humidity_value.isDouble())
                {
                    humidity = humidity_value.toVariant().toInt();
                    qDebug() << "humidity:" << humidity;
                }
            }

            if(obj.contains("light"))
            {
                 QJsonValue light_value = obj.take("light");
                if(light_value.isDouble())
                {
                    light = light_value.toVariant().toInt();
                    qDebug() << "light:" << light;
                }
            }

            if(obj.contains("triaxialX"))
            {
                 QJsonValue triaxialX_value = obj.take("triaxialX");
                if(triaxialX_value.isDouble())
                {
                    triaxialX = triaxialX_value.toVariant().toInt();
                    qDebug() << "triaxialX:" << triaxialX;
                }
            }

            if(obj.contains("triaxialY"))
            {
                 QJsonValue triaxialY_value = obj.take("triaxialY");
                if(triaxialY_value.isDouble())
                {
                    triaxialY = triaxialY_value.toVariant().toInt();
                    qDebug() << "triaxialY:" << triaxialY;
                }
            }

            if(obj.contains("triaxialZ"))
            {
                 QJsonValue triaxialZ_value = obj.take("triaxialZ");
                if(triaxialZ_value.isDouble())
                {
                    triaxialZ = triaxialZ_value.toVariant().toInt();
                    qDebug() << "triaxialZ:" << triaxialZ;
                }
            }

            if(obj.contains("randomCode"))
            {
                 QJsonValue randomCode_value = obj.take("randomCode");
                if(randomCode_value.isString())
                {
                    randomCode = randomCode_value.toString();
                    qDebug() << "randomCode:" << randomCode;
                }
            }

            if(obj.contains("passWord"))
            {
                 QJsonValue passWord_value = obj.take("passWord");
                if(passWord_value.isString())
                {
                    passWord = passWord_value.toString();
                    qDebug() << "passWord:" << passWord;
                }
            }

            if(obj.contains("deviceState"))
            {
                 QJsonValue deviceState_value = obj.take("deviceState");
                if(deviceState_value.isDouble())
                {
                    deviceState = deviceState_value.toVariant().toInt();
                    qDebug() << "deviceState:" << deviceState;
                }
            }

            if(obj.contains("deviceNumber"))
            {
                 QJsonValue deviceNumber_value = obj.take("deviceNumber");
                if(deviceNumber_value.isDouble())
                {
                    deviceNumber = deviceNumber_value.toVariant().toInt();
                    qDebug() << "deviceNumber:" << deviceNumber;
                }
            }

            if(obj.contains("deviceCode"))
            {
                 QJsonValue deviceCode_value = obj.take("deviceCode");
                if(deviceCode_value.isDouble())
                {
                    deviceCode = deviceCode_value.toVariant().toInt();
                    qDebug() << "deviceCode:" << deviceCode;
                }
            }
        }
        qDebug() << endl;
    }

    switch(*funNum)
    {
    case 0x00:  //注册状态记录
        if(stateCode == 1)
        {          
            Register_NO();
          }
        if(stateCode == 0)
        {             
             Register_OK();
        }
        break;

    case 0x01:   //登录状态记录
        if(stateCode == 0)
        {
            chaPasUserToken = userToken;
            Login_OK();
        }
        if(stateCode == 2)
        {
            Login_NO();
        }
        if(stateCode == 3)
        {
            Login_NO();
        }
        break;

    case 0x02:   //忘记密码
        if(stateCode == 0)
        {
            QMessageBox msgBox;
            msgBox.setText("您的验证码为：" + randomCode);  //弹窗显示验证码
            msgBox.exec();
        }
        if(stateCode == 2)
        {
            QMessageBox msgBox;
            msgBox.setText("用户名错误");
            msgBox.exec();
        }
        if(stateCode == 4)
        {
            QMessageBox msgBox;
            msgBox.setText("手机号错误");
            msgBox.exec();
        }
        break;

    case 0x21:   //接收原密码
        {
            QMessageBox msgBox;
            msgBox.setText("您的密码为：" + passWord);  //弹窗显示验证码
            msgBox.exec();
        }
        break;

    case 0x03:  //更改密码
        if(stateCode == 0)
        {
            QMessageBox msgBox;
            msgBox.setText("您的密码修改成功，请妥善保管");
            msgBox.exec();
        }
        if(stateCode == 1)
        {
            QMessageBox msgBox;
            msgBox.setText("对不起，您的密码修改失败");
            msgBox.exec();
        }
        break;

    case 0x04:  //请求获取温度
        if(stateCode == 0)
        {
            TEMP = QString::number(temperature);
        }
        if(stateCode == 1)
        {
            qDebug() << "请求获取温度失败";
        }
        break;

    case 0x05:  //请求获取湿度
        if(stateCode == 0)
        {
            HUMI = QString::number(humidity);
        }
        if(stateCode == 1)
        {
            qDebug() << "请求获取湿度失败";
        }
        break;

    case 0x06:  //请求获取光照
        if(stateCode == 0)
        {
            LIGHT = QString::number(light);
        }
        if(stateCode == 1)
        {
            qDebug() << "请求获取光照失败";
        }
        break;

    case 0x07:  //请求获取三轴数据
        if(stateCode == 0)
        {            
            SANZHOU[0] = triaxialX;
            SANZHOU[1] = triaxialY;
            SANZHOU[2] = triaxialZ;
        }
        if(stateCode == 1)
        {
            qDebug() << "请求获取三轴数据失败";
        }
        break;

    case 0x08:  //打开LED或关闭LED
        if(deviceNumber == 0)
        {
            if(stateCode == 0 && deviceCode == 0)
            {
                 LED1FrameValue = 0;
            }
            if(stateCode == 0 && deviceCode == 1)
            {
                 LED1FrameValue = 1;
            }
            if(stateCode == 1)
            {
                qDebug() << "请求获取LED1失败";
//                LED1FrameValue = 1;
            }
        }
        if(deviceNumber == 1)
        {
            if(stateCode == 0 && deviceCode == 0)
            {
                 LED2FrameValue = 0;
            }
            if(stateCode == 0 && deviceCode == 1)
            {
                 LED2FrameValue = 1;
            }
            if(stateCode == 1)
            {
                qDebug() << "请求获取LED2失败";
//                LED1FrameValue = 1;
            }
        }

        break;

    case 0x09:  //打开关闭风扇
        if(stateCode == 0 && deviceCode == 0 )
        {
            FANFrameValue = 0;
        }
        if(stateCode == 0 && deviceCode == 1)
        {

            FANFrameValue = 1;
        }
        if(stateCode == 1)
        {
            qDebug() << "请求获取风扇的状态失败";
        }
        break;

    case 0x0a:  //打开关闭门
        if(stateCode == 0 && deviceCode == 0)
        {
            DOORFrameValue = 0;
        }
        if(stateCode == 0 && deviceCode == 1)
        {
            DOORFrameValue = 1;
        }
        if(stateCode == 1)
        {
            qDebug() << "请求获取门的状态失败";
        }
        break;

    case 0x0b:  //得到视频文件
        if(stateCode == 0)
        {
            VideoFrameValue = 0;
        }
        if(stateCode == 1)
        {
            qDebug() << "请求获取视频的状态失败";
             VideoFrameValue = 1;
        }
        break;

    default:
        break;
    }
}

void Widget::Register_OK()
{
    qDebug() << "注册成功！" << endl;

    //注册成功弹窗提醒，并关闭当前注册窗口
    QMessageBox msgBox;
    msgBox.setText("          注册成功!\n 您可以继续注册或退出");
    msgBox.exec();

    Register reg;
    reg.close();  //关闭注册类的窗口
}

void Widget::Register_NO()
{
    qDebug() << "注册失败！" << endl;
    //注册失败提醒重新注册
    QMessageBox::warning(this,tr("worning"),\
                         tr("     注册失败\n   请重新注册!"),QMessageBox::Yes,QMessageBox::No);

}

/*****************************************************/
//跳转到智能家居主界面
void Widget::Login_OK()
{
    qDebug() << "登录成功！" << endl;

    SmartHome *SmartHome_ui = new SmartHome;    //新建了一个类的页面
    SmartHome_ui->show();                       //展示智能家居主界面

    qDebug() << "Welcome to SmartHome!" ;
    this->close();
}
void Widget::Login_NO()
{
    qDebug() << "登录失败！" << endl;

    QMessageBox::warning(this,tr("worning"),tr("用户名或密码错误\n   请重新输入!"),QMessageBox::Yes);
    ui->UserNameLineEdit->setFocus();
}
/*****************************************************************/

//设置IP地址与端口号
void Widget::on_SettingPushButton_clicked()
{
    Setting *Setting_ui = new Setting;    //新建了一个类的页面
    Setting_ui->show();
    qDebug() << "Welcome to Setting!" ;
}

void Widget::displayError(QAbstractSocket::SocketError)
{
    qDebug() << tcpSocket->errorString();
}


void Widget::on_ForgetPushButton_clicked()
{
    newConnect();   //建立TCP连接

    ForgetPas *ForgetPas_ui = new ForgetPas;    //新建了一个类的页面
    ForgetPas_ui->show();
    qDebug() << "Welcome to Forget passwd!" ;
}
