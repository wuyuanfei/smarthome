#include "setting.h"
#include "ui_setting.h"

#include <QDebug>
#include <QMessageBox>

QString IP = "127.0.0.1";
int Port = 60000;

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);

    //设置登录主界面的label颜色
    QPalette pen;
    pen.setColor(QPalette::WindowText,Qt::white);
    ui->label->setPalette(pen);
    ui->label_2->setPalette(pen);
    ui->lineEdit->setPalette(pen);
    ui->lineEdit_2->setPalette(pen);

    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/login/images/login/AddUser.png").scaled( // 缩放背景图.
                             this->size(),
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation))); // 使用平滑的缩放方式
    this->setPalette(palette); // 至此, 已给widget加上了背景图.

    //显示默认ip和端口
    ui->lineEdit->setText("192.168.1.155");
    ui->lineEdit_2->setText("9990");
}

Setting::~Setting()
{
    delete ui;
}

void Setting::on_pushButton_clicked()
{
    IP = ui->lineEdit->text();
    Port = ui->lineEdit_2->text().toInt();
    qDebug() << "IP:" << IP;
    qDebug() << "Port:" << Port << endl;

    if((!IP.isEmpty())&&(Port > 0 && Port <=65536)){
        QMessageBox msgBox;
        msgBox.setText("ip地址与端口号修改成功!");
        msgBox.exec();
        this->close();
    }else{
        QMessageBox::warning(this,tr("worning"),\
                             tr("ip地址与端口号修改失败\n   请重新输入!"),QMessageBox::Yes);
    }
}

void Setting::on_pushButton_2_clicked()
{
    this->close();
}
