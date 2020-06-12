/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tetrixboard.h"

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

//    nextPiece.setRandomShape();
    for(bool&b:isFull)
        b = false;
}
//! [0]

//! [1]
void TetrixBoard::setNextPieceLabel(QLabel *label)
{
    nextPieceLabel = label;
}
//! [1]

//! [2]
QSize TetrixBoard::sizeHint() const
{
    return QSize(BoardWidth * 15 + frameWidth() * 2,
                 BoardHeight * 15 + frameWidth() * 2);
}

QSize TetrixBoard::minimumSizeHint() const
//! [2] //! [3]
{
    return QSize(BoardWidth * 5 + frameWidth() * 2,
                 BoardHeight * 5 + frameWidth() * 2);
}
//! [3]

//! [4]
void TetrixBoard::start()
{
    if (isPaused)
        return;
    t= 0;
    isStarted = true;
    isWaitingAfterLine = false;
    numLinesRemoved = 0;
    numPiecesDropped = 0;
    score = 0;
    level = 1;
    clearBoard();

    emit linesRemovedChanged(numLinesRemoved);
    emit scoreChanged(score);
    emit levelChanged(level);


    newPiece();
    timer.start(timeoutTime(), this);
    timer2.start(1000,this);
}
//! [4]

//! [5]
void TetrixBoard::pause()
{
    if (!isStarted)
        return;

    isPaused = !isPaused;
    if (isPaused) {
        timer.stop();
        timer2.stop();
    } else {
        timer.start(timeoutTime(), this);
        timer2.start(1000,this);
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
//! [7]

//    if (isPaused) {
//        painter.drawText(rect, Qt::AlignCenter, tr("Pause"));
//        return;
//    }

//! [8]
    int boardTop = rect.bottom() - BoardHeight*squareHeight();
    if(flash_status!=0)
        qDebug()<<__func__<<"  "<<flash_status;
    for (int i = 0; i < BoardHeight; ++i) {
        for (int j = 0; j < BoardWidth; ++j) {
            TetrixShape shape = shapeAt(j, BoardHeight - i - 1);
            if (shape != NoShape ){
                if(((flash_status&1)  && isFull[BoardHeight - i - 1])){
                    qDebug()<<__func__ << " is full "<<BoardHeight - i - 1;
                    continue;
                }
                drawSquare(painter, rect.left() + j * squareWidth(),
                           boardTop + i * squareHeight(), shape);
            }
        }
//! [8] //! [9]
    }
//! [9]

//! [10]

    for(auto&piece:pieceList)
    if (piece.shape() != NoShape) {
        for (int i = 0; i < 4; ++i) {
            int x = piece.siteX() + piece.x(i);
            int y = piece.siteY() - piece.y(i);
            drawSquare(painter, rect.left() + x * squareWidth(),
                       boardTop + (BoardHeight - y - 1) * squareHeight(),
                       piece.shape());
        }

    }
//    if (curPiece().shape() != NoShape) {
//        for (int i = 0; i < 4; ++i) {
//            int x = curX() + curPiece().x(i);
//            int y = curY() - curPiece().y(i);
//            drawSquare(painter, rect.left() + x * squareWidth(),
//                       boardTop + (BoardHeight - y - 1) * squareHeight(),
//                       curPiece().shape());
//        }
////! [10] //! [11]
//    }
//! [11] //! [12]
}
//! [12]

//! [13]
void TetrixBoard::keyPressEvent(QKeyEvent *event)
{
    if (!isStarted || isPaused || curPiece().shape() == NoShape) {
        QFrame::keyPressEvent(event);
        return;
    }
//! [13]

//! [14]
    switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_Left:
        tryMove(curPiece(), curX() - 1, curY());
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        tryMove(curPiece(), curX() + 1, curY());
        break;

    case Qt::Key_Down:
        tryMove(curPiece().rotatedRight(), curX(), curY());
        break;
    case Qt::Key_W:
    case Qt::Key_Up:
        qDebug()<<__func__<<curX()<<curY();
        tryMove(curPiece().rotatedLeft(), curX(), curY());
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
    if(event->timerId() == timer2.timerId()){
        ++t;
        emit timechanged(timeToString(t));
        if(t%time_gap==0)
            newPiece();
    }
    else if(event->timerId() == flash_timer.timerId()){
        if(flash_status!=no_flash){
            ++flash_status;
            repaint();
            if(flash_status==end_flash){
                timer.start(timeoutTime(), this);
                removeFullLines();
                return;
            }
        }
    }
    else if (event->timerId() == timer.timerId()) {
        /*if (isWaitingAfterLine) {
            isWaitingAfterLine = false;
            timer.start(timeoutTime(), this);
        } else*/
        {
            oneLineDownAll();
        }
    } else{
        QFrame::timerEvent(event);
//! [15] //! [16]
    }
    //! [16] //! [17]
}

TetrixPiece TetrixBoard::getNextPiece() const
{
     TetrixPiece ans;
     ans.setRandomShape();
     return ans;
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
    while (newY > 0) {
        if (!tryMove(curPiece(), curX(), newY - 1))
            break;
        --newY;
        ++dropHeight;
    }
    pieceDropped(dropHeight);
//! [19] //! [20]
}
//! [20]

//! [21]
void TetrixBoard::oneLineDownFirst()
{
    if (!tryMove(curPiece(), curX(), curY() - 1))
        pieceDropped(0);

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
void TetrixBoard::pieceDropped(int dropHeight)
{
    for (int i = 0; i < 4; ++i) {
        int x = curX() + curPiece().x(i);
        int y = curY() - curPiece().y(i);
        shapeAt(x, y) = curPiece().shape();
    }

    ++numPiecesDropped;
//    if (numPiecesDropped % 25 == 0) {
//        ++level;
//        timer.start(timeoutTime(), this);
//        emit levelChanged(level);
//    }
    if(!pieceList.empty())
        pieceList.pop_front();
    score += dropHeight + 7;
    emit scoreChanged(score);
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
        qDebug()<<__func__<<" begin delete";
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
        qDebug()<<__func__<<" begin delete over";
        numLinesRemoved += numFullLines;
        score += 10 * numFullLines;
        emit linesRemovedChanged(numLinesRemoved);
        emit scoreChanged(score);

//        timer.start(timeoutTime(), this);
//        isWaitingAfterLine = true;
//        curPiece().setShape(NoShape);
        update();
    }
//! [28] //! [29]
}
//! [29]

//! [30]
void TetrixBoard::newPiece()
{
    qDebug()<<__func__<<t;
    TetrixPiece piece =getNextPiece();
//    curPiece() = nextPiece;
//    nextPiece.setRandomShape();
//    showNextPiece();
    {
        int curX = BoardWidth / 2 + 1;
        int curY = BoardHeight - 1 + piece.minY();

        piece.siteX()=curX;
        piece.siteY()=curY;
        piece = TetrixPiece(piece);

        pieceList.append(piece);
    }
    if (!tryMove(curPiece(), curX(), curY())) {
//        curPiece().setShape(NoShape);
//        pieceList.front().setShape(NoShape);
        qDebug()<<__func__<<"pieceList.clear()";
        pieceList.clear();
        timer.stop();
        timer2.stop();
        isStarted = false;
    }
//! [30] //! [31]
}
//! [31]

//! [33]

//! [34]
bool TetrixBoard::tryMove(const TetrixPiece &newPiece, int newX, int newY,bool first)
{
    for (int i = 0; i < 4; ++i) {
        int x = newX + newPiece.x(i);
        int y = newY - newPiece.y(i);
//        qDebug()<<__func__<<newX<<newY<<x<<y;
        if (x < 0 || x >= BoardWidth || y < 0 || y >= BoardHeight)
            return false;
        if (shapeAt(x, y) != NoShape)
            return false;
    }
//! [34]
//    qDebug()<<"to be update";
//! [35]

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
void TetrixBoard::drawSquare(QPainter &painter, int x, int y, TetrixShape shape)
{
    static constexpr QRgb colorTable[8] = {
        0x000000, 0xCC6666, 0x66CC66, 0x6666CC,
        0xCCCC66, 0xCC66CC, 0x66CCCC, 0xDAAA00
    };

    QColor color;
    color = colorTable[int(shape)];
    color = Qt::blue;
    color = color.lighter();
    if(shape==TetrixShape::NoShape)
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
    qlonglong h = t2/3600;
    t2 %= 3600;
    qlonglong m = t2/60;
    t2 %= 60;
    QLatin1Char fill('0');
    return QString("%1:%2:%3").arg(h,2,10,fill).arg(m,2,10,fill).arg(t2,2,10,fill);
}
//! [36]
