#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>

int led1 = 0;
int led2 = 0;
int fan = 0;
int door = 0;

QString LoginUserToKen;  //保存登录用户的唯一标识符
QString LoginPas;        //保存登录用户登的密码

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcpServer=new QTcpServer(this);

    QPalette pen;
    pen.setColor(QPalette::WindowText,Qt::white);
    ui->TitleLabel->setPalette(pen);
    ui->NameLabel->setPalette(pen);

    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/new/prefix1/server.png").scaled( // 缩放背景图.
                             this->size(),
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation))); // 使用平滑的缩放方式
    this->setPalette(palette); // 至此, 已给widget加上了背景图.

    userInfoExist = true;       //默认不存在该用户
    temperatureExist = true;
    humidityExist = true;
    lightExist = true;
    triaxialXExist = true;

    // 使用了IPv4的本地主机地址，等价于QHostAddress("127.0.0.1")
    if (!tcpServer->listen(QHostAddress::Any, 60000)) {
        qDebug() << tcpServer->errorString();
        close();
    }

   connect(tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::sendMessage()
{
    // 用于暂存我们要发送的数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    // 设置数据流的版本，客户端和服务器端使用的版本要相同
    out.setVersion(QDataStream::Qt_4_6);
    out << (quint16)0;
    out << tr("hello TCP!!!");
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    // 获取已经建立的连接的套接zi
    // 发送数据成功后，显示提示
    tcpSocket->write(block);
    //ui->label->setText("send message successful!!!");
}


void MainWindow::acceptConnection()
{
    tcpSocket = tcpServer->nextPendingConnection();  //设置监听

    connect(tcpSocket, SIGNAL(readyRead()),this, SLOT(readMessage()));
    connect(tcpSocket,SIGNAL(disconnected()), tcpSocket,SLOT(deleteLater()));

    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));

    // 关闭服务器，不再进行监听
    // tcpServer->close();
}


void MainWindow::readMessage()
{
    Message msg;

    tcpSocket->read((char *)&msg, 4);    //sizeof(msg.type)+sizeof(msg.funNum)+sizeof(msg.length));

    char str[msg.length + 1] = {0};      //多存储一个'\0'
    tcpSocket->read(str, msg.length);

    // 显示接收到的数据
    qDebug() << "---------接收来自客户端的消息---------";
    qDebug("类  型：%#x",msg.type);
    qDebug("功能号：%#x",msg.funNum);
    qDebug("长  度：%d",msg.length);
    qDebug() << "json_str:" << str  << endl;

    if(msg.type == 0xaa){
        switch(msg.funNum)
        {
        case 0x00:
            qDebug() << "**********用户注册**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        case 0x01:
            qDebug() << "**********用户登陆**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        case 0x02:
            qDebug() << "**********忘记密码**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        case 0x03:
            qDebug() << "**********修改密码**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        case 0x21:
            qDebug() << "**********发送原密码**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        case 0x04:
            qDebug() << "**********请求获取温度**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        case 0x05:
            qDebug() << "**********请求获取湿度**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        case 0x06:
            qDebug() << "**********请求获取光照**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        case 0x07:
            qDebug() << "**********请求获取三轴数据**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        case 0x08:
            qDebug() << "**********打开/关闭灯**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        case 0x09:
            qDebug() << "**********打开/关闭风扇**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        case 0x0a:
            qDebug() << "**********打开/关闭门**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        case 0x0b:
            qDebug() << "**********得到视频文件**********";
            analysis_str_form_cli(str,&msg.funNum);
        break;

        default:
            break;
        }

    }else{
        qDebug() << "消息类型不正确！";
    }
}

/****************************************************************
 *
 * 分析来自客户端的json显示
 *
 * **************************************************************/
void MainWindow::analysis_str_form_cli(char *str ,char *msg_funNum)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(str, &json_error);

    //1.解析需要的信息
    QString userName; 				//用户名
    QString oldPassword; 			//旧密码
    QString newPassword; 			//新密码
    QString password; 				//密码
    QString phoneNumber; 			//手机号
    QString userToken;              //用户身份识别码
    QString randomCode; 			//验证码（6 位大小写字母和数字组成）
    int stateCode;                  //状态值（int）
    int temperature;                //温度
    int humidity;                   //湿度
    int triaxialX;                  //三轴 x
    int triaxialY;                  //三轴 y
    int triaxialZ;                  //三轴 z
    int light;                      //光照
    int deviceNumber;               //设备号
    int deviceCode; 				//设备操作码
    int deviceState;                //设备状态码
    int videoList;                  //获取视频文件列表

    qDebug() << "-----服务器Json解析-----";
    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QJsonObject obj = parse_doucment.object();

            if(obj.contains("userName"))
            {
                QJsonValue userName_value = obj.take("userName");
                if(userName_value.isString())
                {
                    userName = userName_value.toString();
                    qDebug() << "userName:" << userName;
                }
            }

            if(obj.contains("password"))
            {
                 QJsonValue password_value = obj.take("password");
                if(password_value.isString())
                {
                    password = password_value.toString();
                    qDebug() << "password:" << password;
                }
            }

            if(obj.contains("phoneNumber"))
            {
                 QJsonValue phoneNumber_value = obj.take("phoneNumber");
                if(phoneNumber_value.isString())
                {
                    phoneNumber = phoneNumber_value.toString();
                    qDebug() << "phoneNumber:" << phoneNumber;
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

            if(obj.contains("deviceNumber"))
            {
                 QJsonValue deviceNumber_value = obj.take("deviceNumber");
                if(deviceNumber_value.isDouble())
                {
                    deviceNumber = deviceNumber_value.toVariant().toInt();
                    qDebug() << "deviceNumber:" << deviceNumber;
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

            if(obj.contains("oldPassword"))
            {
                 QJsonValue oldPassword_value = obj.take("oldPassword");
                if(oldPassword_value.isString())
                {
                    oldPassword = oldPassword_value.toString();
                    qDebug() << "oldPassword:" << oldPassword;
                }
            }

            if(obj.contains("newPassword"))
            {
                 QJsonValue newPassword_value = obj.take("newPassword");
                if(newPassword_value.isString())
                {
                    newPassword = newPassword_value.toString();
                    qDebug() << "newPassword:" << newPassword;
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
    }

    //2.与数据库比对
    Message ConfirmMsg;
    switch(*msg_funNum)
    {
         /************************用户注册********************************************/
        case 0x00:
            if(userInfoExist == false){             //a.数据库中存在，返回错，并返回客户端
                qDebug() << "存在该用户！"<< endl;
                ConfirmMsg.json.insert("stateCode",1);
            }else{                                  //b.数据库中不存在，允许出错，并回发给客户端
                qDebug() << "注册用户成功！"<< endl;
                ConfirmMsg.json.insert("stateCode",0);
            }
        break;

         /************************用户登陆********************************************/
        case 0x01:
            if((userName == "admin") && (password == "admin"))
            {
                LoginPas = password;        //保存登录用户登的密码
                LoginUserToKen = userToken; //保存登录用户的唯一标识符

                qDebug() << "登录信息验证成功！" << endl;
                ConfirmMsg.json.insert("userToken",QString("Qrt3T4"));
                ConfirmMsg.json.insert("stateCode",0);
            }
            else if((userName != "admin") && (password == "admin"))
            {
                qDebug() << "用户名错误！"<< endl;
                ConfirmMsg.json.insert("userToken",QString("null"));
                ConfirmMsg.json.insert("stateCode",2);

            }
            else  if((userName == "admin") && (password != "admin"))
            {
                qDebug() << "密码错误！"<< endl;
                ConfirmMsg.json.insert("userToken",QString("null"));
                ConfirmMsg.json.insert("stateCode",3);
            }
        break;

        /************************忘记密码********************************************/
        case 0x02:
           if((userName == "admin") && (phoneNumber == "18671891024"))
           {
               qDebug() << "用户请求成功！"<< endl;
               ConfirmMsg.json.insert("stateCode",0);
               ConfirmMsg.json.insert("randomCode",QString("qDtT6H"));
           }
           else if((userName != "admin")){
               qDebug() << "用户名错误！"<< endl;
               ConfirmMsg.json.insert("stateCode",2);
           }else if((phoneNumber != "18671891024")){
               qDebug() << "手机号错误！"<< endl;
               ConfirmMsg.json.insert("stateCode",4);
           }
        break;

       /************************发送原密码********************************************/
        case 0x21:
          if((userName == "admin") && (randomCode == "qDtT6H"))
          {
              qDebug() << "用户请求成功！"<< endl;
              ConfirmMsg.json.insert("passWord",QString("admin"));
          }
          else if((userName != "admin")){
              qDebug() << "用户名错误！"<< endl;
              ConfirmMsg.json.insert("stateCode",2);
          }else if((randomCode != "qDtT6H")){
              qDebug() << "验证码错误"<< endl;
              ConfirmMsg.json.insert("stateCode",6);
          }
        break;

      /************************更改密码********************************************/
        case 0x3:
            //当登录密码与旧密码，登录标识符与修改密码标识符一致时方可修改密码
         if(LoginUserToKen == userToken && LoginPas == oldPassword)
         {
             qDebug() << "密码修改成功！"<< endl;
             ConfirmMsg.json.insert("stateCode",0);
             //数据库中更新密码
         }
         else{
             qDebug() << "密码修改失败！"<< endl;
             ConfirmMsg.json.insert("stateCode",1);
         }
        break;

        /************************请求获取温度********************************************/
        case 0x04:
            if(temperatureExist == true){
               qDebug() << "从硬件获取温度成功！"<< endl;
               ConfirmMsg.json.insert("temperature",qrand()%40);
               ConfirmMsg.json.insert("stateCode",0);
            }else{
               qDebug() << "从硬件获取温度失败！"<< endl;
               ConfirmMsg.json.insert("stateCode",1);
            }
        break;

        /************************请求获取湿度********************************************/
        case 0x05:
            if(humidityExist == true){
               qDebug() << "从硬件获取湿度成功！"<< endl;
               ConfirmMsg.json.insert("humidity",qrand()%70 + 10);
               ConfirmMsg.json.insert("stateCode",0);
            }else{
               qDebug() << "从硬件获取湿度失败！"<< endl;
               ConfirmMsg.json.insert("stateCode",1);
            }
        break;

          /************************请求获取光照********************************************/
         case 0x06:
            if(lightExist == true){
               qDebug() << "从硬件获取光照成功！"<< endl;
               ConfirmMsg.json.insert("light",qrand()%950 + 50);
               ConfirmMsg.json.insert("stateCode",0);
            }else{
               qDebug() << "从硬件获取光照失败！"<< endl;
               ConfirmMsg.json.insert("stateCode",1);
            }
         break;

         /************************请求获取三轴数据********************************************/
        case 0x07:
            if(triaxialXExist == true){
                qDebug() << "从硬件获取三轴成功！"<< endl;
                ConfirmMsg.json.insert("triaxialX",qrand()%700 + 50);
                ConfirmMsg.json.insert("triaxialY",qrand()%+400 + 25);
                ConfirmMsg.json.insert("triaxialZ",qrand()%100 + 10);
                ConfirmMsg.json.insert("stateCode",0);
            }else{
                qDebug() << "从硬件获取三轴失败！"<< endl;
                ConfirmMsg.json.insert("stateCode",1);
            }
        break;

        /************************打开/关闭灯********************************************/
       case 0x08:
           if(deviceNumber == 0){
               led1 = ~led1;
               if(led1){
                   qDebug() << "从硬件获取灯状态成功！"<< endl;
                   ConfirmMsg.json.insert("stateCode",0);
                   ConfirmMsg.json.insert("deviceState",0);
                   ConfirmMsg.json.insert("deviceNumber",0);
               }else{
                   qDebug() << "从硬件获取灯状态失败！"<< endl;
                   ConfirmMsg.json.insert("stateCode",1);
                   ConfirmMsg.json.insert("deviceState",1);
                   ConfirmMsg.json.insert("deviceNumber",0);
               }
           }
           if(deviceNumber == 1){
               led2 = ~led2;
               if(led2){
                   qDebug() << "从硬件获取灯状态成功！"<< endl;
                   ConfirmMsg.json.insert("stateCode",0);
                   ConfirmMsg.json.insert("deviceState",0);
                   ConfirmMsg.json.insert("deviceNumber",1);
               }else{
                   qDebug() << "从硬件获取灯状态失败！"<< endl;
                   ConfirmMsg.json.insert("stateCode",1);
                   ConfirmMsg.json.insert("deviceState",1);
                   ConfirmMsg.json.insert("deviceNumber",1);
               }
           }

       break;

       /************************打开/关闭风扇********************************************/
      case 0x09:
         fan = ~fan;
          if(fan){
              qDebug() << "从硬件获取风扇状态成功！"<< endl;
              ConfirmMsg.json.insert("stateCode",0);
              ConfirmMsg.json.insert("deviceState",0);
          }else{
              qDebug() << "从硬件获取风扇状态失败！"<< endl;
              ConfirmMsg.json.insert("stateCode",1);
               ConfirmMsg.json.insert("deviceState",1);
          }
      break;

      /************************打开/关闭门********************************************/
     case 0x0a:
        door = ~door;
         if(door){
             qDebug() << "从硬件获取门状态成功！"<< endl;
             ConfirmMsg.json.insert("stateCode",0);
             ConfirmMsg.json.insert("deviceState",0);
         }else{
             qDebug() << "从硬件获取门状态失败！"<< endl;
             ConfirmMsg.json.insert("stateCode",1);
             ConfirmMsg.json.insert("deviceState",1);
         }
     break;

    default:
        break;

    }

/*****************************************************************/
    //3.回发确认包
    //填充登录信息类
    ConfirmMsg.type = 0xff;
    ConfirmMsg.funNum = *msg_funNum;

    //json对象转QString
    QJsonDocument document1;
    document1.setObject(ConfirmMsg.json);
    QByteArray byte_array1 = document1.toJson(QJsonDocument::Compact);
    QString json_str1(byte_array1);

    //QString转char *
    QByteArray ba1 = json_str1.toLatin1();
    char *confirm_str = ba1.data();
    ConfirmMsg.length = json_str1.length();

    //发送数据包到APP端
    tcpSocket->write((char *)&ConfirmMsg,4);     //type + funnum + length
    tcpSocket->write(confirm_str,ConfirmMsg.length);  //json

    //发送数据成功后，显示提示
    qDebug() << "---------服务器回发确认消息---------";
    qDebug("类  型：%#x",ConfirmMsg.type);
    qDebug("功能号：%#x",ConfirmMsg.funNum);
    qDebug("长  度：%d",ConfirmMsg.length);
    qDebug() << "json_str:" << confirm_str;
    qDebug() <<"确认信息发送成功" << endl;
}

void MainWindow::displayError(QAbstractSocket::SocketError)
{
    qDebug() << tcpSocket->errorString();
}

