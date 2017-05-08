#include "register.h"
#include "ui_register.h"
#include "message.h"
#include "widget.h"

#include <QDebug>
#include <QMessageBox>
#include <QJsonDocument>

extern QTcpSocket *tcpSocket;

Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);

    //设置登录主界面的label颜色
    QPalette pen;
    pen.setColor(QPalette::WindowText,Qt::white);
    ui->UserNamelabel->setPalette(pen);
    ui->PasswdLabel->setPalette(pen);
    ui->AffirmLabel->setPalette(pen);
    ui->PhoneLabel->setPalette(pen);
    ui->groupBox->setPalette(pen);

    ui->PasswdLineEdit->setEchoMode(QLineEdit::Password);//设置密码编辑行为暗码
    ui->AffirmLineEdit->setEchoMode(QLineEdit::Password);//设置密码编辑行为暗码
	ui->PhoneLineEdit->setMaxLength(11); //设置电话的最大长度为11位

    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/login/images/login/AddUser.png").scaled( // 缩放背景图.
                             this->size(),
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation))); // 使用平滑的缩放方式
    this->setPalette(palette); // 至此, 已给widget加上了背景图.
}


Register::~Register()
{

    delete ui;
}


void Register::on_CancelPushButton_clicked()
{
    this->close();
}

void Register::on_OKPushButton_clicked()
{
        /*
         * 1.填充用户名，密码，手机号
         * 2.发送用户名和密码到服务器
         * 3.对服务器收到的回执进行判断
         * */
        UserName = ui->UserNamelineEdit->text();
        Passwd = ui->PasswdLineEdit->text();
        ConfirmPas = ui->AffirmLineEdit->text();
        Phone = ui->PhoneLineEdit->text();

        qDebug() << " ---------------注册信息---------------";
        qDebug() << " UserName :" << UserName;
        qDebug() << "  Passwd  :" << Passwd;
        qDebug() << "ConfirmPas:" << ConfirmPas;
        qDebug() << "   Phone  :" << Phone << endl;

        message RegisterMsg;

        //填充注册信息类
        //只有当（用户名，密码，确认密码均有效，并且两次输入的密码一样，手机号为11为时）才能发生注册信息
        if(UserName.isEmpty()){
            QMessageBox msgBox;
            msgBox.setText("用户名不能为空");
            msgBox.exec();
        }else if(Passwd != ConfirmPas){
            QMessageBox msgBox;
            msgBox.setText("新密码与确认密码不一致，请重新输入");
            msgBox.exec();
            ui->AffirmLineEdit->setText("");
            ui->PasswdLineEdit->setText("");
        }else if(Phone.length()!=11){
            QMessageBox msgBox;
            msgBox.setText("请输入正确11位手机号码");
            msgBox.exec();
            ui->PhoneLineEdit->setText("");
        }else if(Passwd.isEmpty() | ConfirmPas.isEmpty()){
            QMessageBox msgBox;
            msgBox.setText("密码不能为空");
            msgBox.exec();
            ui->AffirmLineEdit->setText("");
            ui->PasswdLineEdit->setText("");
        }
		/*else if((Passwd.length() < 8) | (ConfirmPas.length() < 8)){
            QMessageBox msgBox;
            msgBox.setText("密码长度不能小于8位");
            msgBox.exec();
            ui->PhoneLineEdit->setText("");
        }*/
		else{
            RegisterMsg.type = 0xaa;
            RegisterMsg.funNum = 0x00;
            RegisterMsg.json.insert("userName",UserName);
            RegisterMsg.json.insert("password",Passwd);
            RegisterMsg.json.insert("phoneNumber",Phone);

            //json对象转QString
            QJsonDocument document;
            document.setObject(RegisterMsg.json);
            QByteArray byte_array = document.toJson(QJsonDocument::Compact);
            QString json_str(byte_array);

            //QString转char *
            QByteArray ba = json_str.toLatin1();
            char *str = ba.data();
            RegisterMsg.length = json_str.length();

            //发送数据包到服务器端
            tcpSocket->write((char *)&RegisterMsg,4);     //type + funnum + length
            tcpSocket->write(str,RegisterMsg.length);     //json

            //发送数据成功后，显示提示
            qDebug() << "---------发送消息到服务器---------";
            qDebug("类  型：%#x",RegisterMsg.type);
            qDebug("功能号：%#x",RegisterMsg.funNum);
            qDebug("长  度：%d",RegisterMsg.length);
            qDebug() << "json_str:" << str;
            qDebug() <<"注册信息发送成功" << endl;
        }
}
