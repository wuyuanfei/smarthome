#include "camera.h"
#include "ui_camera.h"

#include <QDir>
#include <QFileDialog>
#include <QTimer>
#include <QDateTime>

extern QString date_time_str;

camera::camera(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::camera)
{
    ui->setupUi(this);

    //设置界面的label颜色
    QPalette pen;
    pen.setColor(QPalette::WindowText,Qt::white);

    //标题,时间,标题栏
    ui->Titlelabel->setPalette(pen);
    ui->TimeLabel->setPalette(pen);
    ui->StatusLabel->setPalette(pen);
    ui->groupBox->setPalette(pen);
    ui->groupBox_2->setPalette(pen);

    //视频监控界面背景设置
    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/main/images/main/home_bh.png").scaled( // 缩放背景图.
                             this->size(),
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation))); // 使用平滑的缩放方式
    this->setPalette(palette); // 至此, 已给widget加上了背景图.


    //视频监控设置
    cameraer = new QCamera(this);
    viewfinder = new QCameraViewfinder(this);
    imageCapture = new QCameraImageCapture(cameraer);

    ui->horizontalLayout->addWidget(viewfinder);
    ui->StatusLabel->setScaledContents(true);

    cameraer->setViewfinder(viewfinder);
    cameraer->start();

    //视频监控
    connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), \
            this, SLOT(displayImage(int,QImage)));

    connect(ui->CapturePushButton, SIGNAL(clicked()), this, SLOT(captureImage()));
    connect(ui->SavePushButton, SIGNAL(clicked()), this, SLOT(saveImage()));
    connect(ui->ExitPushButton, SIGNAL(clicked()), cameraer, SLOT(stop()));
    connect(ui->ExitPushButton, SIGNAL(clicked()), this, SLOT(close()));

    //实时显示时间
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this, SLOT(updateTime()));
    timer->start(1000);
}

void camera::updateTime()
{
    ui->TimeLabel->setText(date_time_str);                  //在标签上显示时间
}

camera::~camera()
{
    delete ui;
}

/***********************************************************************************/
/*
 * 视频监控
 * */
void camera::captureImage()
{
    ui->StatusLabel->setText("正在捕获图片");
    imageCapture->capture();
}

void camera::displayImage(int /*id*/, QImage image)
{
    ui->DisplayLabel->setPixmap(QPixmap::fromImage(image).scaled(ui->DisplayLabel->size(),
                                                                 Qt::IgnoreAspectRatio,
                                                                 Qt::SmoothTransformation));
    ui->StatusLabel->setText("捕获成功");
}

void camera::saveImage()
{
    QString fileName=QFileDialog::getSaveFileName(this, tr("保存到文件"), QDir::homePath(), tr("jpeg格式文件(*.jpg)"));
    if(fileName.isEmpty()) {
        ui->StatusLabel->setText("保存操作已取消");
        return;
    }

    const QPixmap* pixmap=ui->DisplayLabel->pixmap();
    if(pixmap) {
        pixmap->save(fileName);
        ui->StatusLabel->setText("保存成功");
    }
}

void camera::closeEvent(QCloseEvent *)
{
    cameraer->stop();
    this->close();
}

/***********************************************************************************/
/*
 * 天气与位置
 *
void camera::getLatAndLon(void)
{
    m_networkGetLatAndLon.get(QNetworkRequest(QUrl(QString("http://api.map.baidu.com/location/ip?ak=%1&coor=bd09ll").arg("54GzQbyspseUfRUbvDdVMKQW")))); // 这里可以修改百度LBS的key
}

void camera::getWeatherByGeographic(const double &lat, const double &lon)
{
    m_networkGetWeather.get(QNetworkRequest(QUrl(QString("http://api.openweathermap.org/data/2.5/weather?lat=%1&lon=%2").arg(lat).arg(lon))));
}

void camera::startInquiry(void)
{
    this->getLatAndLon();
}

void camera::onGetLatAndLon(QNetworkReply *reply)
{
    QJsonObject data = QJsonDocument::fromJson(reply->readAll()).object();
    QJsonObject xy = data.value("content").toObject().value("point").toObject();
    double lat = xy.value("y").toString().toDouble();
    double lon = xy.value("x").toString().toDouble();

    if(lat && lon)
    {
        this->getWeatherByGeographic(lat, lon);
    }
    else
    {
        qDebug("Get weather fail");
        emit finished("Get lat and lon fail", QJsonObject());
    }
}

void camera::onGetWeather(QNetworkReply *reply)
{
    QJsonObject data = QJsonDocument::fromJson(reply->readAll()).object();

    if(!data.contains("message"))
    {
        emit finished("OK", data);
    }
    else
    {
        qDebug("Get weather fail");
        emit finished("Get weather fail: " + data.value("message").toString(), QJsonObject());
    }
}
***********************************************************************/
