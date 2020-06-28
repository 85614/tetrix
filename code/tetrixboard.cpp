
#include "tetrixboard.h"
#include<QDebug>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>


TetrixBoard::TetrixBoard(QWidget *parent)
    : QFrame(parent), isStarted(false), isPaused(false)
{
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setFocusPolicy(Qt::StrongFocus);
    clearBoard();
    for(bool&b:isFull)
        b = false;

}

QSize TetrixBoard::sizeHint() const
{
    return QSize(BoardWidth * 15 + frameWidth() * 2,
                 BoardHeight * 15 + frameWidth() * 2);
}

QSize TetrixBoard::minimumSizeHint() const
{
    return QSize(BoardWidth * 5 + frameWidth() * 2,
                 BoardHeight * 5 + frameWidth() * 2);
}
//游戏开始
void TetrixBoard::start()
{
    if (isPaused)
        return;
    t= 0;
    isStarted = true;
    numLinesRemoved = 0;
    numPiecesDropped = 0;
    score = 0;
    clearBoard();

    emit timechanged("00:00");
    emit piecesRemovedChanged(numPiecesDropped);
    emit scoreChanged(score);
    emit levelChanged(_Level);


    newPiece();
//
    drop_timer.start(dropTime(), this);
    timer_per_second.start(1000/speed_rate,this);
    level(hard);
}

void TetrixBoard::pause()
{
    qDebug()<<"尝试暂停";
    if (!isStarted)
        return;

    isPaused = !isPaused;
    if (isPaused) {
        drop_timer.stop();
        timer_per_second.stop();
    } else {
        drop_timer.start(dropTime(), this);
        timer_per_second.start(1000/speed_rate,this);
    }
    update();

}


void TetrixBoard::paintEvent(QPaintEvent *event)
{

    if(!pieceList.empty()){
//        qDebug()<<curX()<<curY();
    }
    QFrame::paintEvent(event);
    QPainter painter(this);
    QRect rect = contentsRect();

    int boardTop = rect.bottom() - BoardHeight*squareHeight();
    if(flash_status!=0)
        qDebug()<<__func__<<"  "<<flash_status;
    for (int i = 0; i < BoardHeight; ++i) {
        for (int j = 0; j < BoardWidth; ++j) {
            BlockShape shape = shapeAt(j, BoardHeight - i - 1);
            if (shape != NoShape ){
                if(((flash_status&1)  && isFull[BoardHeight - i - 1])){
                    //闪烁的暗时间
                    continue;
                }
                drawSquare(painter, rect.left() + j * squareWidth(),
                           boardTop + i * squareHeight(), shape);
            }
        }
    }
    //绘制所有未下落的方块
    for(auto&piece:pieceList)
        if (piece.shape() != NoShape) {
            for (int i = 0; i < 4; ++i) {
                int x = piece.siteX() + piece.getX(i);
                int y = piece.siteY() - piece.getY(i);
                drawSquare(painter, rect.left() + x * squareWidth(),
                           boardTop + (BoardHeight - y - 1) * squareHeight(),
                           piece.shape());
            }

        }

}

void TetrixBoard::keyPressEvent(QKeyEvent *event)
{
    if (!isStarted || isPaused || curPiece().shape() == NoShape) {
        QFrame::keyPressEvent(event);
        return;
    }
//改了一下键
    switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_Left:
        tryMoveLeft();
//        tryMove(curPiece(), curX() - 1, curY());
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
//        tryMove(curPiece(), curX() + 1, curY());
        tryMoveRight();
        break;

    case Qt::Key_Down:
        rotatedRight();
//        tryMove(curPiece().rotatedRight(), curX(), curY());
        break;
    case Qt::Key_W:
    case Qt::Key_Up:
        rotatedLeft();
//        tryMove(curPiece().rotatedLeft(), curX(), curY());
        break;
    case Qt::Key_Space:
        dropDown();
        break;
//    case Qt::Key_D:
    case Qt::Key_S:
        oneLineDownFirst();
        break;
    default:
        QFrame::keyPressEvent(event);
    }

}


void TetrixBoard::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timer_per_second.timerId()){
        //是每秒的计时事件
        ++t;
        emit timechanged(timeToString(t));
        if(t% pieceOccerTime() ==0)
            newPiece();
        //=================
    }
    else if(event->timerId() == flash_timer.timerId()){
        //是闪烁的计事件
        if(flash_status!=no_flash){
            ++flash_status;
            repaint();
            if(flash_status==end_flash){
                drop_timer.start(dropTime(), this);
                removeFullLines();
                return;
            }
        }
    }
    else if (event->timerId() == drop_timer.timerId()) {
        //是下落的计时事件
            oneLineDownAll();
    } else{
        //父类处理
        QFrame::timerEvent(event);
    }

}

TetrixPiece TetrixBoard::getNextPiece() const
{
     ReadLine();
     TetrixPiece ans;
     //ans.setRandomShape();
     ans.getnewShape();
     return ans;
}



void TetrixBoard::clearBoard()
{
    for (int i = 0; i < BoardHeight * BoardWidth; ++i)
        board[i] = NoShape;
    pieceList.clear();
}

void TetrixBoard::dropDown()
{
    int dropHeight = 0;
    int newY = curY();
    qDebug()<<pieceList.empty()<<" "<<"newY"<<newY;
    while (newY > 0) {
        if (!tryMove(curPiece(), curX(), newY - 1))
            break;
        --newY;
        ++dropHeight;
    }
    pieceDropped();

}

void TetrixBoard::oneLineDownFirst()
{
    if (!tryMove(curPiece(), curX(), curY() - 1))
        pieceDropped();
    if(dropcount>1){
        emit piecesRemovedChanged(numPiecesDropped);
    }
    ++dropcount;
}

TetrixBoard::Level TetrixBoard::level(TetrixBoard::Level newLevel){
    Level old = level();_Level = newLevel;
    emit levelChanged(newLevel);
    _Level = newLevel;

    qDebug()<<__func__<<1;
    if(drop_timer.isActive())
    {
        qDebug()<<__func__<<2;
        drop_timer.stop();
        drop_timer.start(dropTime()/speed_rate,this);
        qDebug()<<dropTime();
    }
    return old;
}

bool TetrixBoard::isFilled(int x, int y)
{
    int pos = x*BoardWidth + y;
    if (board[pos]==NoShape)
        return false;
    else return true;
}
void TetrixBoard::oneLineDownAll(){

    for(TetrixPiece&piece:pieceList){
        if(&piece==&curPiece())
            oneLineDownFirst();
        else
            tryMove(piece,piece.siteX(),piece.siteY()-1,false);
    }
}


void TetrixBoard::pieceDropped()
{
    for (int i = 0; i < 4; ++i) {
        int x = curX() + curPiece().getX(i);
        int y = curY() - curPiece().getY(i);
        shapeAt(x, y) = curPiece().shape();
    }

    ++numPiecesDropped;
    if(!pieceList.empty()){
        pieceList.pop_front();
        dropcount = 0;
    }
    emit piecesRemovedChanged(numPiecesDropped);

    removeFullLines();


}



void TetrixBoard::removeFullLines()
{
    //mycode
    if(flash_status==no_flash)
        for (int i = BoardHeight - 1; i >= 0; --i) {
            int j =0;
            for ( j = 0; j < BoardWidth; ++j) {
                if (shapeAt(j, i) == NoShape) {
                    break;
                }
            }
            if(j==BoardWidth)
            {
                isFull[i]=true;
                flash_status = need_flash;
            }
        }


    if(flash_status!=no_flash) {
        switch (flash_status) {
        case end_flash:
            break;
        case need_flash:
            flash_timer.start(100,this);
            return;
        default:
            return;
        }
        flash_status = no_flash;
        flash_timer.stop();
//        qDebug()<<__func__<<" begin delete";
        int numFullLines=0;
        for(int i = 0;i<BoardHeight;){
            if(isFull[i])
            {
                ++numFullLines;
                for (int k = i; k < BoardHeight - 1; ++k) {
                    for (int j = 0; j < BoardWidth; ++j)
                    {
                        shapeAt(j, k) = shapeAt(j, k + 1);
                        isFull[k]=isFull[k+1];
                    }
                }
                for (int j = 0; j < BoardWidth; ++j){
                    shapeAt(j, BoardHeight - 1) = NoShape;
                    isFull[BoardHeight-1]=false;
                }
            }
            else {
                ++i;
            }
        }
//        qDebug()<<__func__<<" begin delete over";
        numLinesRemoved += numFullLines;
        score += BoardWidth * numFullLines;
        emit piecesRemovedChanged(numPiecesDropped);
        emit scoreChanged(score);
        update();
    }

}

void TetrixBoard::newPiece()
{
//    qDebug()<<__func__<<t;
    TetrixPiece piece =getNextPiece();
    {
        int curX = BoardWidth / 2 ;
        int curY = BoardHeight - 1 + piece.getUpBound();

        piece.siteX()=curX;
        piece.siteY()=curY;
        piece = TetrixPiece(piece);

        pieceList.append(piece);
    }
    if (!tryMove(curPiece(), curX(), curY())) {
        qDebug()<<__func__<<"pieceList.clear()";
        pieceList.clear();
        drop_timer.stop();
        timer_per_second.stop();
        isStarted = false;
    }
}

bool TetrixBoard::tryMove(const TetrixPiece &newPiece, int newX, int newY,bool first)
{
    for (int i = 0; i < 4; ++i) {
        int x = newX + newPiece.getX(i);
        int y = newY - newPiece.getY(i);
        if (x < 0 || x >= BoardWidth || y < 0 || y >= BoardHeight)
            return false;
        if (shapeAt(x, y) != NoShape)
            return false;
    }
    TetrixPiece &piece = const_cast<TetrixPiece&>(newPiece);
    piece.siteX() = newX;
    piece.siteY() = newY;
    if(first)
        curPiece()=newPiece;
    update();
    return true;
}

void TetrixBoard::drawSquare(QPainter &painter, int x, int y, BlockShape shape)
{
    static constexpr QRgb colorTable[8] = {
        0x000000, 0xCC6666, 0x66CC66, 0x6666CC,
        0xCCCC66, 0xCC66CC, 0x66CCCC, 0xDAAA00
    };

    QColor color;
    color = colorTable[int(shape)];
    color = Qt::blue;
    color = color.lighter();
    if(shape==BlockShape::NoShape)
        color = QRgb(0x000000);
    painter.fillRect(x + 1, y + 1, squareWidth() - 2, squareHeight() - 2,
                     color);

    painter.setPen(color.lighter());
    painter.drawLine(x, y + squareHeight() - 1, x, y);
    painter.drawLine(x, y, x + squareWidth() - 1, y);

    painter.setPen(color.darker());
    painter.drawLine(x + 1, y + squareHeight() - 1,
                     x + squareWidth() - 1, y + squareHeight() - 1);
    painter.drawLine(x + squareWidth() - 1, y + squareHeight() - 1,
                     x + squareWidth() - 1, y + 1);
}



QString TetrixBoard::timeToString(qlonglong t2)
{
    qlonglong m = t2/60;
    t2 %= 60;
    QLatin1Char fill('0');
    return QString("%1:%2").arg(m,2,10,fill).arg(t2,2,10,fill);
}

