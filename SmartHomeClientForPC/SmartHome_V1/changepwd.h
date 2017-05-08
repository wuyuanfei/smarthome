#ifndef CHANGEPWD_H
#define CHANGEPWD_H

#include <QWidget>

namespace Ui {
class ChangePwd;
}

class ChangePwd : public QWidget
{
    Q_OBJECT

public:
    explicit ChangePwd(QWidget *parent = 0);
    ~ChangePwd();

private slots:
    void on_OkpushButton_clicked();

    void on_CancelpushButton_clicked();

private:
    Ui::ChangePwd *ui;

    QString oldPassword;
    QString newPassword;
    QString ConfirmnewPassword;
};

#endif // CHANGEPWD_H
