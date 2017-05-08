#ifndef FORGETPAS_H
#define FORGETPAS_H

#include <QWidget>

namespace Ui {
class ForgetPas;
}

class ForgetPas : public QWidget
{
    Q_OBJECT

public:
    explicit ForgetPas(QWidget *parent = 0);
    ~ForgetPas();

    void SendModifyInfo(QString str_type,QString modify_str,unsigned char msg_type);

private slots:
    void on_ConfirmPushButton_clicked();

    void on_SendPushButton_clicked();

private:
    Ui::ForgetPas *ui;

    QString UserName;
    QString phoneNumber;
    QString randomCode;
};

#endif // FORGETPAS_H
