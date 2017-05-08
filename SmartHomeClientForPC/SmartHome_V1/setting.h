#ifndef SETTING_H
#define SETTING_H

#include <QWidget>

namespace Ui {
class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = 0);
    ~Setting();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Setting *ui;
};

#endif // SETTING_H
