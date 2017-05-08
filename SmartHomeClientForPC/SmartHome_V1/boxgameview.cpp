#include "boxgameview.h"
#include "onebox.h"
#include "boxgroup.h"

#include <QMediaPlayer>
#include <QDir>
#include <QLabel>

BoxGameView::BoxGameView()
{
    setWindowTitle("智能家居-使徒行者");
    this->setFixedSize(816,510);
    this->setAlignment(Qt::AlignLeft|Qt::AlignTop);

    /***********************游戏背景音乐***************/
    musicPlayer = new QMediaPlayer(this);
    musicPlayer->setMedia(QUrl::fromLocalFile("E:/qt_project/SmartHome_V1/sounds/Heaven.mp3"));
    musicPlayer->setVolume(70); //设置声音大小

    musicPlayer->play();
    /******************************************************/

    QGraphicsScene *scene = new QGraphicsScene;
    scene->setSceneRect(0,0,800,500);
    scene->setBackgroundBrush(QPixmap(":/games/images/games/background01.png"));
    setCacheMode(QGraphicsView::CacheBackground);//缓存背景图片

    topline = new QGraphicsLineItem(197,47,403,47);
    topline->setPen(QPen(Qt::red));
    rightline = new QGraphicsLineItem(403,47,403,453);
    rightline->setPen(QPen(Qt::green));
    bottomline = new QGraphicsLineItem(403,453,197,453);
    bottomline->setPen(QPen(Qt::yellow));
    leftline = new QGraphicsLineItem(197,47,197,453);
    leftline->setPen(QPen(Qt::blue));

    scene->addItem(topline);
    scene->addItem(rightline);
    scene->addItem(bottomline);
    scene->addItem(leftline);


    label = new QGraphicsTextItem("智能家居娱乐系统——简易俄罗斯方块");
    label->setPos(150,0);
    label->setHtml("<font color=yellow>智能家居娱乐系统——简易俄罗斯方块</font>");
    label->setFont(QFont("华文行楷",25));

    scoreText = new QGraphicsTextItem("score:0");
    scoreText->setPos(550,200);
    scoreText->setHtml("<font color=green>0</font>");
    scoreText->setFont(QFont("times",30));

    gameoverText = new QGraphicsTextItem("GAME OVER");
    gameoverText->setFont(QFont("times",20));
    gameoverText->setHtml("<font color=red>GAME OVER!</font>");
    gameoverText->setPos(500,300);

    gameoverText->hide();

    scene->addItem(scoreText);
    scene->addItem(gameoverText);
    scene->addItem(label);

    currentGroup = new BoxGroup();
    currentGroup->createBoxGroup();
    currentGroup->setPos(300,70);
    currentGroup->setFocus();
    currentGroup->startMove(true);//启动下移
    scene->addItem(currentGroup);


    nextGroup = new BoxGroup();
    nextGroup->createBoxGroup();
    nextGroup->setPos(500,100);
    nextGroup->startMove(false);//不启动下移
    scene->addItem(nextGroup);

    this->setScene(scene);

    score = 0;

    connect(currentGroup,SIGNAL(fullRows()),this,SLOT(clearFullRows()));
    connect(currentGroup,SIGNAL(gameover()),this,SLOT(gameoverSlot()));
}

void BoxGameView::clearFullRows()
{
    for(int y=449;y>50;y -= 20){
        //返回参数指定矩形区域内所有的item
        QList<QGraphicsItem *>list = scene()->items(199,y,202,22,Qt::ContainsItemShape,Qt::DescendingOrder);
        if(list.count()==10){//满一行
            foreach (QGraphicsItem*item, list) {
                OneBox *box = (OneBox*)item;
                box->deleteLater();

            }
            rows << y;//保存满行的位置
        }
    }

    if(rows.count() > 0)//有满行
    {
        //更新分数
        score += rows.count()*10;
        scoreText->setHtml("<font color=green>"+ QString::number(score)+"</font>");
        //将满行上面的所有图元往下移动一个单位
        moveBoxs();

    }else
    {
        currentGroup->setPos(300,70);
        currentGroup->createBoxGroup(nextGroup->currentShape);
        currentGroup->startMove(true);

        nextGroup->clearBoxGroup(true);
        nextGroup->createBoxGroup(BoxGroup::RandomShape);
        nextGroup->setPos(500,100);
    }
}

 void BoxGameView::moveBoxs()
 {
     //从位于最上面的满行开始移
     for(int i=rows.count();i>0;--i){
         int row = rows.at(i-1);//获取最上面的满行的y坐标
         //获取移动区域上边框和最上面的满行之间的所有的item
         QList<QGraphicsItem*> list = scene()->items(199,49,202,row-47,Qt::IntersectsItemShape,Qt::DescendingOrder);
         //将满行上面的所有的item下移一格
         foreach(QGraphicsItem* item,list){
             item->moveBy(0,20);
         }
     }
     //将满行列表清除为0
     rows.clear();

     //填充curreGroup新的图形
     currentGroup->setPos(300,70);
     currentGroup->createBoxGroup(nextGroup->currentShape);
     currentGroup->startMove(true);

     //先销毁nextGroup 四个item，填充新的图形
     nextGroup->clearBoxGroup(true);
     nextGroup->createBoxGroup(BoxGroup::RandomShape);
     nextGroup->setPos(500,100);
 }

 void BoxGameView::gameoverSlot()
 {
    score = 0;
    gameoverText->show();
 }

 void BoxGameView::closeEvent(QCloseEvent *event)
 {
     Q_UNUSED(event);
     musicPlayer->stop();
 }

