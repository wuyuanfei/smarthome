#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QProcess>
#include <QSpinBox>
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QGridLayout>
#include <QVideoWidget>

namespace Ui {
class VideoPlayer;
}

class VideoPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayer(QWidget *parent = 0);
    ~VideoPlayer();

    void play(QString fileName);

private slots:
    void on_PlayPushButton_clicked();

    void on_StopPushButton_clicked();

    void on_LastPushButton_clicked();

    void on_NextPushButton_clicked();

    void on_AddPushButton_clicked();

    void on_DelPushButton_clicked();

    void on_TimeJumpPushButton_clicked();

//    void on_SpeedSettingPushButton_clicked();

//    void on_listWidget_doubleClicked(QListWidgetItem *index);

//    void on_ProcessHorizontalSlider_sliderMoved(int position);

//    void on_VolumeHorizontalSlider_sliderMoved(int position);

    void sureBtnClicked();

//    void dataRecieve();

private:
    Ui::VideoPlayer *ui;

    QMediaPlayer    *player;
    QMediaPlaylist  *playlist;
    QVideoWidget    *videowidget;
    int playindex;

    QSpinBox    *MinBox;
    QSpinBox    *SecBox;
    QString     CurrentPercent;
    QProcess    *Process;
    QString     CurrentStr;
    QString     TotalTime;

protected:
    void closeEvent(QCloseEvent *);
};

#endif // VIDEOPLAYER_H
