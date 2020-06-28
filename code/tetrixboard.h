

#ifndef TETRIXBOARD_H
#define TETRIXBOARD_H

#include <QBasicTimer>
#include <QFrame>
#include <QPointer>
#include <QDebug>
#include "tetrixpiece.h"

#include <QList>
QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE


//! [0]
class TetrixBoard : public QFrame
{
    Q_OBJECT

public:
    enum { BoardWidth = 10, BoardHeight = 20 };
    enum Level {easy,normal,hard};//难度
    TetrixBoard(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void dropDown();//直接下落
    void oneLineDownFirst();//最下方的方块下落一格

    Level level(){return _Level;}//难度
    Level level(Level newLevel);
    bool tryMoveLeft(){return tryMove(curPiece(), curX() - 1, curY());}
    bool tryMoveRight(){return tryMove(curPiece(), curX() + 1, curY());}
    bool rotatedRight(){return tryMove(curPiece().rightRotate(), curX(), curY());}
    bool rotatedLeft(){return tryMove(curPiece().leftRotate(), curX(), curY());}

    int getScore(){return score;}
    TetrixPiece getCurPiece(){return curPiece();}
    bool isFilled(int x,int y);


public slots:
    void start();
    void pause();
    void changeFile();

signals:
    void scoreChanged(int score);
    void levelChanged(int _Level);
    void piecesRemovedChanged(int numLines);
    void timechanged(const QString &s);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
//! [0]

//! [1]
private:

    int speed_rate =1;//speed_rate倍的速度


    TetrixPiece getNextPiece()const;//用于重新实现以完成文件输入，未完成

    BlockShape &shapeAt(int x, int y) { return board[(y * BoardWidth) + x]; }
    int dropTime() { static constexpr int dropTimes[3]{500,300,200};return dropTimes[_Level]/speed_rate ; }
    int pieceOccerTime(){static constexpr int times[]{5,3,1};return times[_Level];}
    int squareWidth() { return contentsRect().width() / BoardWidth; }//一格的宽
    int squareHeight() { return contentsRect().height() / BoardHeight; }//一格的高
    void clearBoard();//清除所有方块及列表中的方块
    void oneLineDownAll();//列表中所有的方块下降一格
    void pieceDropped();//确定某个方块已经落下，可以操纵下一个方块

    void newPiece();//出现新的方块
    bool tryMove(const TetrixPiece &newPiece, int newX, int newY,bool first=true);
    //尝试在某个位置放置一个方块，若first为真，将newPiece赋值给最下面的方块（大多是因为newPiece是最下面那个方块旋转后生成的新方块）
    //first为假，则不可能newPiece和pieceList里的方块不是同一个方块，因为只能旋转最下面的方块

    void drawSquare(QPainter &painter, int x, int y, BlockShape shape);//画一格正方形


    QBasicTimer drop_timer;//用于下落的计时器
    bool isStarted;
    bool isPaused;


    QList<TetrixPiece> pieceList;//当前已出现未落下的方块的列表
    TetrixPiece noShpePiece;
    //下面三个只是因为pieceList的出现，直接把原来的变量批量改成函数（省事）
    TetrixPiece& curPiece(){return pieceList.empty()?noShpePiece:pieceList.front();}
    int& curX(){return curPiece().siteX();}
    int& curY(){return curPiece().siteY();}

    int numLinesRemoved;//已填满的行数
    int numPiecesDropped;//已落下的方块数，
    int score;//分数，文件2.1题目描述说“程序的得分是所消解的栅格的总数”
    Level _Level = easy ;//难度,需要变成“容易”“适中”“挑战”来显示，未完成
    BlockShape board[BoardWidth * BoardHeight];
    //其实它这个Shape只是为了颜色，不要也行
    //我这里只区分了noShape和其他


    //闪烁部分
    void removeFullLines();//清除满了的行，分两步，第一次调用先存到isFull中，闪烁后，再次调用这个函数才会清除
    QBasicTimer flash_timer;
    enum{no_flash,need_flash,flash_1,flash_2,flash_3,end_flash};
    //no_flash 表示没有行被填满
    //end_flash表示闪烁结束
    //其他只有计数的作用
    int flash_status = no_flash;
    bool isFull[BoardHeight];//记录每一行是否满

    //时间部分
    //下降间隔计时

    QBasicTimer timer_per_second;//每秒计时
    qlonglong t=0;//游戏的时间
    QString timeToString(qlonglong t);//时间转成字符串格式

    int dropcount = 0;
};
//! [1]

#endif



