#ifndef IMAGE_H
#define IMAGE_H

#include <QMainWindow>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPaintEvent>

namespace Ui {
class Image;
}

class Image : public QMainWindow
{
    Q_OBJECT

public:
    explicit Image(QWidget *parent = 0);
    ~Image();

private:
    Ui::Image *ui;

/*************图片浏览***********************/
protected:
    void paintEvent (QPaintEvent * event);
private:
    void SetBgImage(const QString &strPath);
private slots:
    void Slot_ItemClicked(QListWidgetItem * item);
private:
    QListWidget *m_pListWidget;
    QString m_strPath;
};

#endif // IMAGE_H
