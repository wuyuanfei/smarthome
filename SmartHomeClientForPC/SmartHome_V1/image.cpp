#include "image.h"
#include "ui_image.h"

const int W_ICONSIZE = 96; //图片宽度
const int H_ICONSIZE = 96; //图片高度
const int H_ITEMSIZE = 110; //单元项高度(因为单元项既包括图片又包括文本)

Image::Image(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Image)
{
    ui->setupUi(this);

    //创建QListWidget部件
    m_pListWidget = new QListWidget(this);
    //设置QListWidget中的单元项的图片大小
    m_pListWidget->setIconSize(QSize(W_ICONSIZE, H_ICONSIZE));
    m_pListWidget->setResizeMode(QListView::Adjust);
    //设置QListWidget的显示模式
    m_pListWidget->setViewMode(QListView::IconMode);
    //设置QListWidget中的单元项不可被拖动
    m_pListWidget->setMovement(QListView::Static);
    //设置QListWidget中的单元项的间距
    m_pListWidget->setSpacing(10);

    //依次创建7个单元项
    for(int nIndex = 0;nIndex < 7;++nIndex)
    {
        //获得图片路径
        QString strPath=QString(":/list/images/list/3.jpg").arg(nIndex+1);
        //生成图像objPixmap
        QPixmap objPixmap(strPath);
        //生成QListWidgetItem对象(注意：其Icon图像进行了伸缩[96*96])---scaled函数
        QListWidgetItem *pItem = new QListWidgetItem(QIcon(objPixmap.scaled(QSize(W_ICONSIZE,H_ICONSIZE))),"animal tiger pig");
        //设置单元项的宽度和高度
        pItem->setSizeHint(QSize(W_ICONSIZE,H_ITEMSIZE));
        m_pListWidget->insertItem(nIndex, pItem);

    }

    setCentralWidget(m_pListWidget);

    //设置信号槽
    connect(m_pListWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(Slot_ItemClicked(QListWidgetItem*)));

    m_strPath = "";

    setWindowTitle("图片浏览器-使徒行者");
}

Image::~Image()
{
    delete ui;
}

//设置主窗口背景
void Image::SetBgImage(const QString &strPath)
{
    QPixmap objPixmap(strPath);
    QPalette palette = this->palette();
    if(strPath.isEmpty())
    {
        palette.setBrush(QPalette::Base, QBrush(QColor(35,197,148)));
    }
    else
    {
        palette.setBrush(QPalette::Base, QBrush(objPixmap.scaled(width(),height())));
    }
    setPalette(palette);
}

//槽函数
void Image::Slot_ItemClicked(QListWidgetItem * item)
{
    if(!item)
    {
        return;
    }
    int nRowIndex = m_pListWidget->row(item);

    m_strPath=QString(":/list/images/list/3.jpg").arg(nRowIndex+1);
    update();
}
void Image::paintEvent (QPaintEvent * event)
{
    Q_UNUSED(event);
    SetBgImage(m_strPath);
}


