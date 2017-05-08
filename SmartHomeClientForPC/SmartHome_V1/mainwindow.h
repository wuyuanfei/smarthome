#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QMediaPlayer"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void qmpPlayOrPause();
    void qmpSetVol();
    void qmpSetVal();
    void qmpSelectFile();
    void qmpPlays();
    void qmpPlay();
    void qmpPlaying(qint64 position);
    void qmpDuration(qint64 pos);
    void qmpLast();
    void qmpNext();
    void qmpPause();
    void qmpSettitle();
    void closeEvent(QCloseEvent *event);

protected:
    void dragEnterEvent(QDragEnterEvent *e); //拖动进入事件
    void dropEvent(QDropEvent *e); //放下事件

private:
    Ui::MainWindow *ui;
    bool qmpPlayPause;
    int nums;
    QMediaPlayer* qmpPlayer;
    QStringList songlst;
    QString songname;
};

#endif // MAINWINDOW_H
