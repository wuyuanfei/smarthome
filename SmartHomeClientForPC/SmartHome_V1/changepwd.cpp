#include "changepwd.h"
#include "ui_changepwd.h"
#include "widget.h"

#include "message.h"

#include <QMessageBox>
#include <QJsonDocument>

extern QTcpSocket *tcpSocket;

QString chaPasUserToken;

ChangePwd::ChangePwd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChangePwd)
{
    ui->setupUi(this);

    //设置界面的label颜色
    QPalette pen;
    pen.setColor(QPalette::WindowText,Qt::white);
    ui->NewPasswdLabel->setPalette(pen);
    ui->OldPasswdLabel->setPalette(pen);
    ui->ConfirmNewPasswdLineEdit->setPalette(pen);
    ui->groupBox->setPalette(pen);

    ui->NewPasswdLineEdit->setEchoMode(QLineEdit::Password);//设置密码编辑行为暗码
    ui->OldPasswdLineEdit->setEchoMode(QLineEdit::Password);//设置密码编辑行为暗码
    ui->ConfirmNewPasswdLineEdit->setEchoMode(QLineEdit::Password);//设置密码编辑行为暗码

    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/login/images/login/AddUser.png").scaled( // 缩放背景图.
                             this->size(),
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation))); // 使用平滑的缩放方式
    this->setPalette(palette); // 至此, 已给widget加上了背景图.
}

ChangePwd::~ChangePwd()
{
    delete ui;
}

void ChangePwd::on_OkpushButton_clicked()
{
    oldPassword = ui->OldPasswdLineEdit->text();
    newPassword = ui->NewPasswdLineEdit->text();
    ConfirmnewPassword = ui->ConfirmNewPasswdLineEdit->text();

    message ChaPasMsg;
    //填充修改密码信息类
    ChaPasMsg.type = 0xaa;
    ChaPasMsg.funNum = 03;
    ChaPasMsg.json.insert("userToken",chaPasUserToken);
    ChaPasMsg.json.insert("oldPassword",oldPassword);

    if(newPassword == ConfirmnewPassword && newPassword != oldPassword){
        ChaPasMsg.json.insert("newPassword",newPassword);

        //json对象转QString
        QJsonDocument document;
        document.setObject(ChaPasMsg.json);
        QByteArray byte_array = document.toJson(QJsonDocument::Compact);
        QString json_str(byte_array);

        //QString转char *
        QByteArray ba = json_str.toLatin1();
        char *str = ba.data();
        ChaPasMsg.length = json_str.length();

        //发送数据包到服务器端
        tcpSocket->write((char *)&ChaPasMsg,4);     //type + funnum + length
        tcpSocket->write(str,ChaPasMsg.length);          //json

        //发送数据成功后，显示提示
        qDebug() << "---------发送消息到服务器---------";
        qDebug("类  型：%#x",ChaPasMsg.type);
        qDebug("功能号：%#x",ChaPasMsg.funNum);
        qDebug("长  度：%d",ChaPasMsg.length);
        qDebug() << "json_str:" << str;
        qDebug() <<"修改密码信息发送成功" << endl;
    }else if(newPassword == oldPassword){
        //两次输入的密码不一致，提醒重新输入
        QMessageBox::warning(this,tr("worning"),\
                             tr("新密码不能与原密码相同，请重新输入新密码!"),\
                             QMessageBox::Yes,QMessageBox::No);

        ui->OldPasswdLineEdit->setText("");
        ui->NewPasswdLineEdit->setText("");
        ui->ConfirmNewPasswdLineEdit->setText("");
    }else if(newPassword != ConfirmnewPassword){
        //两次输入的密码不一致，提醒重新输入
        QMessageBox::warning(this,tr("worning"),\
                             tr("新密码与确认密码不一致，请重新输入密码!"),\
                             QMessageBox::Yes,QMessageBox::No);

        ui->OldPasswdLineEdit->setText("");
        ui->NewPasswdLineEdit->setText("");
        ui->ConfirmNewPasswdLineEdit->setText("");
    }
}

void ChangePwd::on_CancelpushButton_clicked()
{
    this->close();
}
