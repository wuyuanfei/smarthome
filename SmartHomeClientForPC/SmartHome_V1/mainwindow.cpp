#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "QFile"
#include "QFileDialog"
#include "QMediaPlayer"
#include "QMediaPlaylist"
#include "QTimer"
#include "QTime"
#include "QDragEnterEvent"
#include "QMimeData"
#include "QList"
#include "QUrl"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qmpPlayPause = false;

    QPalette pen;
    pen.setColor(QPalette::WindowText,Qt::white);
    ui->lbltitle->setPalette(pen);

    this->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/main/images/main/home_bh.png").scaled( // 缩放背景图.
                             this->size(),
                             Qt::IgnoreAspectRatio,
                             Qt::SmoothTransformation))); // 使用平滑的缩放方式
    this->setPalette(palette); // 至此, 已给widget加上了背景图.

    qmpPlayer = new QMediaPlayer;
    ui->slsndvalue->setValue(qmpPlayer->volume());

    connect(qmpPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(qmpPlaying(qint64)));    
    connect(qmpPlayer,SIGNAL(durationChanged(qint64)),this,SLOT(qmpDuration(qint64)));
}

void MainWindow::qmpPlayOrPause()
{
    if(!qmpPlayPause)
        qmpPause();
    else if(qmpPlayPause)
        qmpPlays();
}

void MainWindow::qmpSetVol()
{
    qmpPlayer->setVolume(ui->slsndvalue->value());
}

void MainWindow::qmpSetVal()
{
    if(ui->lbltime->text() != "00:00")
    qmpPlayer->setPosition(ui->slvalue->value());
}

void MainWindow::qmpSelectFile()
{
    QString n_dir,q_filename;
    q_filename = QFileDialog::getOpenFileName(this,"Open","","Sounds Files (*.mp3)");
    if(!q_filename.isNull())
    {
       n_dir = q_filename.left(q_filename.lastIndexOf("/"));

       ui->txtpath->setText(q_filename);
       ui->txtpath->setEnabled(false);

       QDir qd(n_dir);
       qd.setFilter(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
       qd.setSorting(QDir::Size|QDir::Reversed);
       QFileInfoList list = qd.entryInfoList();
       for(int i=0;i< list.size();++i)
       {
           if(list.at(i).fileName().right(3) == "mp3")
           {
               songlst += n_dir + "/" + list.at(i).fileName();
               ui->lstsong->addItem(list.at(i).fileName());
           }
       }
    }
}

void MainWindow::qmpLast()
{
    if(ui->lbltime->text() != "00:00")
    {
        if(ui->lstsong->currentIndex().row() == 0)
            nums = songlst.size() - 1;
        else
            nums = ui->lstsong->currentIndex().row() - 1;
        qmpPlayer->setMedia(QMediaContent(songlst.at(nums)));
        ui->lstsong->setCurrentRow(nums);
        qmpPause();qmpSettitle();
    }
}

void MainWindow::qmpNext()
{
    if(ui->lbltime->text() != "00:00")
    {
    if(ui->lstsong->currentIndex().row() == songlst.size() - 1)
        nums = 0;
    else
        nums = ui->lstsong->currentIndex().row() + 1;
    qmpPlayer->setMedia(QMediaContent(songlst.at(nums)));
    ui->lstsong->setCurrentRow(nums);
    qmpPause();qmpSettitle();
    }
}

void MainWindow::qmpPlays()
{
    qmpPlayer->pause();
    qmpPlayPause = false;
    ui->btnplaynpause->setStyleSheet(tr("border-image: \
    url(:/video/images/video/play.png);"));
}

void MainWindow::qmpPause()
{
    qmpPlayer->play();
    qmpPlayPause = true;
     ui->btnplaynpause->setStyleSheet(tr("border-image: \
    url(:/video/images/video/pause.png);"));
}

void MainWindow::qmpPlay()
{
    qmpPlayer->setMedia(QMediaContent(songlst.at(ui->lstsong->currentIndex().row())));
    qmpPause();qmpSettitle();
}

void MainWindow::qmpSettitle()
{
    this->setWindowTitle("使徒行者 - " + ui->lstsong->currentItem()->text());
}

void MainWindow::qmpPlaying(qint64 position)
{
    if(position == qmpPlayer->duration())qmpNext();
    ui->slvalue->setSliderPosition(position);
    QTime duration(0, position / 60000, qRound((position % 60000) / 1000.0));
    ui->lbltime->setText(duration.toString("mm:ss"));
}

void MainWindow::qmpDuration(qint64 pos)
{
    ui->slvalue->setRange(0,pos);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasUrls())
        e->acceptProposedAction();
    else e->ignore();
}

void MainWindow::dropEvent(QDropEvent *e)
{
    if(e->mimeData()->hasUrls())
    {
        QList<QUrl> urls = e->mimeData()->urls();
        for(int i=0;i< urls.size();i++)
        {
            if(urls.at(i).toString().right(3) == "mp3")
            {
                QString qstr = urls.at(i).toString().mid(8,urls.at(i).toString().length() - 8);
                songlst += qstr;
                qDebug() << qstr;
                ui->lstsong->addItem(urls.at(i).fileName());
            }
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
     qmpPlayer->stop();
}

MainWindow::~MainWindow()
{
    delete ui;
}
