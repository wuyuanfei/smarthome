#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>

#include <QCamera>              //用于打开系统的摄像头设备
#include <QCameraViewfinder>    //用于显示捕获的视频
#include <QCameraImageCapture>  //用于截图

//#include <QJsonObject>          //获取天气与位置所需类
//#include <QJsonDocument>
//#include <QNetworkAccessManager>
//#include <QNetworkReply>

namespace Ui {
class camera;
}

class camera : public QWidget
{
    Q_OBJECT

public:
    explicit camera(QWidget *parent = 0);
    ~camera();

//    void getLatAndLon(void);
//    void getWeatherByGeographic(const double &lat, const double &lon);

public slots:
//    void startInquiry(void);


private slots:
    void captureImage();
    void displayImage(int,QImage);
    void saveImage();
    void updateTime();

//    void onGetLatAndLon(QNetworkReply *reply);
//    void onGetWeather(QNetworkReply *reply);


signals:
//    void finished(const QString message, const QJsonObject weatherData);

private:
    Ui::camera *ui;

    QCamera *cameraer;
    QCameraViewfinder *viewfinder;
    QCameraImageCapture *imageCapture;

//    QNetworkAccessManager m_networkGetLatAndLon;
//    QNetworkAccessManager m_networkGetWeather;

protected:
    void closeEvent(QCloseEvent *);

};

#endif // CAMERA_H
