#include "videoplayer.h"
#include "ui_videoplayer.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

VideoPlayer::VideoPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoPlayer)
{
    ui->setupUi(this);

    QPalette pen;
    pen.setColor(QPalette::WindowText,Qt::white);
    ui->TitleLabel->setPalette(pen);
    //ui->VolumeLabel->setPalette(pen);

    //主界面背景设置
    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/main/images/main/home_bh.png").scaled( // 缩放背景图.
                             this->size(),
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation))); // 使用平滑的缩放方式
    this->setPalette(palette); // 至此, 已给widget加上了背景图.

    //视频播放设置
    playindex = 1;
    player = new QMediaPlayer(this,0);
    playlist = new QMediaPlaylist(this);
    videowidget = new QVideoWidget(this);
//       nextBtn = new QPushButton(tr("next"), this);
//       QGridLayout *mainLayout = new QGridLayout(this);

    //C:/Users/Yeapht/Desktop/smartHome/SmartHome_V1_APP/SmartHome_V1/video
    playlist->addMedia(QUrl::fromLocalFile("C:/Users/Yeapht/Desktop/smartHome/SmartHome_V1_APP/SmartHome_V1/video/Alan-Walker-Faded.mp4"));
    playlist->addMedia(QUrl::fromLocalFile("C:/Users/Yeapht/Desktop/smartHome/SmartHome_V1_APP/SmartHome_V1/video/Alan-Walker-Alone.mp4"));
    playlist->addMedia(QUrl::fromLocalFile("C:/Users/Yeapht/Desktop/smartHome/SmartHome_V1_APP/SmartHome_V1/video/Alan-Walker-Sing-Me-to-Sleep.mp4"));

    playlist->setCurrentIndex(playindex);
    player->setPlaylist(playlist);
    player->setVideoOutput(videowidget);
    videowidget->show();

    ui->horizontalLayout->addWidget(videowidget);
//       mainLayout->addWidget(videowidget, 1, 1, 3, 3);
//       mainLayout->addWidget(nextBtn, 4, 1, 1, 1);
//       this->setLayout(mainLayout);

    player->setVolume(100);

    Process =new QProcess(this);
    connect(Process,SIGNAL(clicked()),this,SLOT(on_PlayPushButton_clicked()));
}

VideoPlayer::~VideoPlayer()
{
    delete ui;
}

int Playkey = 0;
void VideoPlayer::on_PlayPushButton_clicked()
{
    if(Playkey++%2 == 0){
        player->play();
        ui->PlayPushButton->setStyleSheet(tr("border-image: \
        url(:/video/images/video/play.png);"));
    }else{
        player->pause();
        ui->PlayPushButton->setStyleSheet(tr("border-image: \
        url(:/video/images/video/pause.png);"));
    }


//    Process->write("pause\n");
//    if(ui->PlayPushButton->text()=="播放")
//    {
//        connect(Process,SIGNAL(readyReadStandardOutput()),this,SLOT(dataRecieve()));
//        Process->write("get_time_length\n");
//        Process->write("get_time_pos\n");
//        Process->write("get_percent_pos\n");
//        ui->PlayPushButton->setText("暂停");
//    }
//    else
//    {
//        disconnect(Process,SIGNAL(readyReadStandardOutput()),this,SLOT(dataRecieve()));
//        ui->PlayPushButton->setText("播放");
//    }
}

void VideoPlayer::on_StopPushButton_clicked()
{
    player->stop();

//   Process->write("quit\n");
//   ui->TimeLabel->clear();
//   ui->PercentLabel->clear();
}

void VideoPlayer::on_LastPushButton_clicked()
{
    if(playindex == 1){
        playindex = 3;
    }else{
        playindex--;
    }
    player->stop();
    playlist->setCurrentIndex(playindex);
    player->play();

//    if(ui->listWidget->currentRow()==0){
//        ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
//    }else{
//        ui->listWidget->setCurrentRow(ui->listWidget->currentRow()-1);
//    }
    //play(ui->listWidget->currentItem()->text());
//    ui->PlayPushButton->setText("暂停");
}

void VideoPlayer::on_NextPushButton_clicked()
{
    //ui->PlayPushButton->setText("暂停");
    if(playindex == 3){
        playindex = 1;
    }else{
        playindex++;
    }
    player->stop();
    playlist->setCurrentIndex(playindex);
    player->play();

//    if(ui->listWidget->currentRow()==ui->listWidget->count()-1){
//        ui->listWidget->setCurrentRow(0);
//    }else{
//        ui->listWidget->setCurrentRow(ui->listWidget->currentRow()+1);
//    }
    //play(ui->listWidget->currentItem()->text());
}

void VideoPlayer::on_AddPushButton_clicked()
{
//    QStringList fileNames = QFileDialog::getOpenFileNames(this,"choose movies","/","Movie (*.m4v *.mp4 *.avi *.wmv *.mpg)");
//    if(fileNames.count()!=0){
//        ui->listWidget->addItems(fileNames);
//    }
}

void VideoPlayer::on_DelPushButton_clicked()
{
//    if(ui->listWidget->currentRow()==-1){
//       QMessageBox::warning(this,"提示","未选中项目或列表为空",QMessageBox::Yes);
//    }else{
//     ui->listWidget->takeItem(ui->listWidget->currentRow());
//    }
}

void VideoPlayer::on_TimeJumpPushButton_clicked()
{
    QDialog *timeDialog=new QDialog(this);
    timeDialog->resize(300,100);
    MinBox=new QSpinBox(timeDialog);
    MinBox->setGeometry(QRect(20,20,50,30));
    QLabel *minLabel=new QLabel("分",timeDialog);
    minLabel->setGeometry(QRect(80,20,20,30));

    SecBox=new QSpinBox(timeDialog);
    SecBox->setGeometry(QRect(100,20,50,30));
    QLabel *secLabel=new QLabel("秒",timeDialog);
    secLabel->setGeometry(QRect(150,20,30,30));

    QPushButton *sureBtn=new QPushButton("确定",timeDialog);
    sureBtn->setGeometry(QRect(180,20,70,30));
    connect(sureBtn,SIGNAL(clicked()),this,SLOT(sureBtnClicked()));
    timeDialog->show();
}

void VideoPlayer::sureBtnClicked()
{
    int jumpTime = MinBox->value()*60+SecBox->value();
    Process->write(QString("seek "+QString::number(jumpTime)+" 2\n").toUtf8());
}

#if 0
void VideoPlayer::on_SpeedSettingPushButton_clicked()
{
    double speed = QInputDialog::getDouble(this,"播放速度设置","相对于正常速度倍数:");
    if(speed > 0)
    Process->write(QString("speed_set "+QString::number(speed)+" 2\n").toUtf8());
}

void VideoPlayer::on_listWidget_doubleClicked(QListWidgetItem  *index)
{
    play(index->text());
    ui->PlayPushButton->setText("暂停");
}

void VideoPlayer::play(QString fileName)
{
    QStringList args;

    Process->kill();

    Process =new QProcess(this);
    connect(Process,SIGNAL(readyReadStandardOutput()),this,SLOT(dataRecieve()));

    args<<"-slave";
    args<<"-quiet";
    args<<"-wid"<<QString::number(ui->view->winId(),10);
    args<<"-zoom";
    args<<"-x";
    args<<"370";
    args<<"-y";
    args<<"335";
    args<<"-vo";
    args<<"x11";
    args<< fileName;

    Process->start("mplayer",args);
    ui->PlayPushButton->setEnabled(true);
    ui->StopPushButton->setEnabled(true);
}

void VideoPlayer::on_ProcessHorizontalSlider_sliderMoved(int position)
{
    Process->write(QString("seek "+QString::number(position)+" 2\n").toUtf8());
}

void VideoPlayer::on_VolumeHorizontalSlider_sliderMoved(int position)
{

}

void VideoPlayer::dataRecieve()
{
    Process->write("get_time_length\n");
    Process->write("get_time_pos\n");
    Process->write("get_percent_pos\n");

    while(Process->canReadLine())
    {
        QByteArray b = Process->readLine();
        if(b.startsWith("ANS_TIME_POSITION"))
        {
            b.replace(QByteArray("\n"),QByteArray(""));
            QString s(b);
            CurrentStr=s.mid(18);
            ui->ProcessHorizontalSlider->setValue(s.mid(18).toFloat());    //更新进度条
        }
        else if((b.startsWith("ANS_LENGTH")))
        {
            b.replace(QByteArray("\n"),QByteArray(""));
            QString s(b);
            TotalTime=s.mid(11);
            ui->TimeLabel->setText(CurrentStr+"秒/"+TotalTime+"秒  "); //显示时间进度
            ui->ProcessHorizontalSlider->setRange(0,s.mid(11).toFloat());
        }
        else if((b.startsWith("ANS_PERCENT_POSITION")))
        {
            b.replace(QByteArray("\n"),QByteArray(""));
            QString s(b);
            CurrentPercent=s.mid(21);
            ui->PercentLabel->setText("("+CurrentPercent+"%)");
        }
    }
}


#endif

void VideoPlayer::closeEvent(QCloseEvent *)
{
    player->stop();
    this->close();
}


