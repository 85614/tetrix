
#include "tetrixboard.h"
#include<QDebug>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>

//! [0]
TetrixBoard::TetrixBoard(QWidget *parent)
    : QFrame(parent), isStarted(false), isPaused(false)
{
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setFocusPolicy(Qt::StrongFocus);
    clearBoard();
    level(hard);
    for(bool&b:isFull)
        b = false;
    if(_Level==easy){
        time_gap = 5;
        qDebug()<<"难度为简单！";
    }
    else if(_Level==normal){
        time_gap = 3;
        qDebug()<<"难度为普通！";
    }
    else if(_Level==hard){
        time_gap = 1;
        qDebug()<<"难度为困难！";
    }
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
//! [3]

//! [4]
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
    timer.start(dropTime(), this);
    timer_per_second.start(1000,this);
}
//! [4]

//! [5]
void TetrixBoard::pause()
{
    qDebug()<<"尝试暂停";
    if (!isStarted)
        return;

    isPaused = !isPaused;
    if (isPaused) {
        timer.stop();
        timer_per_second.stop();
    } else {
        timer.start(dropTime(), this);
        timer_per_second.start(1000,this);
    }
    update();
    //! [5] //! [6]
}

//! [6]

//! [7]
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
//! [12]

//! [13]
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
//! [14]
}

//! [15]
void TetrixBoard::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timer_per_second.timerId()){
        //是每秒的计时事件
        ++t;
        emit timechanged(timeToString(t));
        if(t%time_gap==0)
            newPiece();
        //=================
    }
    else if(event->timerId() == flash_timer.timerId()){
        //是闪烁的计事件
        if(flash_status!=no_flash){
            ++flash_status;
            repaint();
            if(flash_status==end_flash){
                timer.start(dropTime(), this);
                removeFullLines();
                return;
            }
        }
    }
    else if (event->timerId() == timer.timerId()) {
        //是下落的计时事件
            oneLineDownAll();
    } else{
        //父类处理
        QFrame::timerEvent(event);
    }
    //! [16] //! [17]
}



//! [17]

//! [18]
void TetrixBoard::clearBoard()
{
    for (int i = 0; i < BoardHeight * BoardWidth; ++i)
        board[i] = NoShape;
    pieceList.clear();
}
//! [18]

//! [19]
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
//! [19] //! [20]
}
//! [20]

//! [21]
void TetrixBoard::oneLineDownFirst()
{
    if (!tryMove(curPiece(), curX(), curY() - 1))
        pieceDropped();
    if(dropcount>1){
        emit piecesRemovedChanged(numPiecesDropped);
    }
    ++dropcount;
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

//! [21]

//! [22]
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

//! [22] //! [23]
}
//! [23]

//! [24]
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

//! [28]
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
//! [28] //! [29]
}
//! [29]

//! [30]
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
        timer.stop();
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
//! [35]

//! [36]
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
//! [36]
