#include "forgetpas.h"
#include "ui_forgetpas.h"
#include "message.h"
#include "widget.h"

#include <QJsonDocument>

extern QTcpSocket *tcpSocket;

ForgetPas::ForgetPas(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ForgetPas)
{
    ui->setupUi(this);

    QPalette pen;
    pen.setColor(QPalette::WindowText,Qt::white);
    ui->UserNameLabel->setPalette(pen);
    ui->PhoneLabel->setPalette(pen);
    ui->ConfirmLabel->setPalette(pen);

    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/login/images/login/AddUser.png").scaled( // 缩放背景图.
                             this->size(),
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation))); // 使用平滑的缩放方式
    this->setPalette(palette); // 至此, 已给widget加上了背景图.

}

void ForgetPas::on_ConfirmPushButton_clicked()
{
    UserName = ui->UserNameLineEdit->text();
    phoneNumber = ui->PhoneLineEdit->text();
    SendModifyInfo(UserName,phoneNumber,0x02);
}

void ForgetPas::on_SendPushButton_clicked()
{
    phoneNumber = ui->PhoneLineEdit->text();
    UserName = ui->UserNameLineEdit->text();
    randomCode = ui->ConfirmPhoneLineEdit->text();
    SendModifyInfo(UserName,phoneNumber,0x21);
}

void ForgetPas::SendModifyInfo(QString UserName,QString phoneNumber,unsigned char msg_type)
{
    message ForgetPasMsg;
    //填充修改密码信息类
    ForgetPasMsg.type = 0xaa;
    ForgetPasMsg.funNum = msg_type;
    if(msg_type == 0x02){
        ForgetPasMsg.json.insert("userName",UserName);
        ForgetPasMsg.json.insert("phoneNumber",phoneNumber);
    }else if(msg_type == 0x21){
        ForgetPasMsg.json.insert("userName",UserName);
        ForgetPasMsg.json.insert("randomCode",randomCode);
    }


    //json对象转QString
    QJsonDocument document;
    document.setObject(ForgetPasMsg.json);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    QString json_str(byte_array);

    //QString转char *
    QByteArray ba = json_str.toLatin1();
    char *str = ba.data();
    ForgetPasMsg.length = json_str.length();

    //发送数据包到服务器端
    tcpSocket->write((char *)&ForgetPasMsg,4);     //type + funnum + length
    tcpSocket->write(str,ForgetPasMsg.length);          //json

    //发送数据成功后，显示提示
    qDebug() << "---------发送消息到服务器---------";
    qDebug("类  型：%#x",ForgetPasMsg.type);
    qDebug("功能号：%#x",ForgetPasMsg.funNum);
    qDebug("长  度：%d",ForgetPasMsg.length);
    qDebug() << "json_str:" << str;
    qDebug() <<"忘记密码信息发送成功" << endl;
}

ForgetPas::~ForgetPas()
{
    delete ui;
}


