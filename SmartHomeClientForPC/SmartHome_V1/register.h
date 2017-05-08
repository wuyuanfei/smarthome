#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = 0);
    ~Register();

private slots:
    void on_CancelPushButton_clicked();

    void on_OKPushButton_clicked();

private:
    Ui::Register *ui;

    QString UserName;
    QString Passwd;
    QString ConfirmPas;
    QString Phone;

signals:
    void create_connect();
};

#endif // REGISTER_H
