#ifndef SMARTHOME_H
#define SMARTHOME_H

#include <QWidget>
#include <QMediaPlayer>

namespace Ui {
class SmartHome;
}

class SmartHome : public QWidget
{
    Q_OBJECT

public:
    explicit SmartHome(QWidget *parent = 0);
    ~SmartHome();

    void time_display();

    void Delay_MSec_Suspend(unsigned int msec);  //自实现延时函数

    void play_music();

    void send_request(unsigned char funcNum);

    void sleep(int msec);

private slots:
    void updateTime();

    void on_DeviePushButton_clicked();

    void on_EntertainmentPushButton_clicked();

    void on_VideoPushButton_clicked();

    void on_MultiMediaPushButton_clicked();

    void on_AdminPushButton_clicked();

    void on_MainPushButton_clicked();

    void on_MsgPushButton_clicked();

    void on_pushButton_clicked();

    void showSensor();
    void send_temp_request();
    void send_humi_request();
    void send_light_request();
    void send_sanzhou_request();

    void on_TempPushButton_clicked();

    void on_HumiPushButton_clicked();

    void on_LightPushButton_clicked();

    void on_TraPushButton_clicked();

private:
    Ui::SmartHome *ui;

    QMediaPlayer *musicPlayer;

    QTimer *timer_sensor1;
    QTimer *timer_sensor2;
    QTimer *timer_sensor3;
    QTimer *timer_sensor4;

protected:
    void closeEvent(QCloseEvent *);

};

#endif // SMARTHOME_H
