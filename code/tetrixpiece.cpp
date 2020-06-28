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

#include "tetrixpiece.h"
#include<stdlib.h>
#include<qdebug.h>
//#include <QtCore>

//! [0]
void TetrixPiece::setRandomShape()
{
    //随机设置一种形状，这里+1是为了跳过第一个NoShape
    BlockShape ranShape = BlockShape(rand()%5+1);
    setShape(ranShape);
}
//! [0]

//! [1]
void TetrixPiece::setShape(BlockShape shape)
{
    static constexpr int coordsTable[6][4][2] = {
        //NoShape
        {{0,0},{0,0},{0,0},{0,0}},
        //LineShape
        {{0,0},{0,1},{0,2},{0,3}},
        //LShape
        {{0,0},{1,0},{1,1},{1,2}},
        //TShape
        {{0,0},{0,1},{0,2},{1,1}},
        //ZShape
        {{0,0},{0,1},{1,1},{1,2}},
        //OShape
        {{0,0},{0,1},{1,0},{1,1}}
    };

    for (int i = 0; i < 4 ; i++) {
        for (int j = 0; j < 2; ++j)
            coords[i][j] = coordsTable[shape][i][j];
    }
    blockShape = shape;
//! [1] //! [2]
}

int TetrixPiece::getX(int index) const { return coords[index][0]; }

int TetrixPiece::getY(int index) const { return coords[index][1]; }
//! [2]

//! [3]
int TetrixPiece::getLeftBound() const
{
    int leftbound = coords[0][0];
    for (int i = 0; i < 4; i++)
    {
        if (coords[i][0]<leftbound)
        {
            leftbound = coords[i][0];
        }
    }
    return leftbound;
}

int TetrixPiece::getRightBound() const
//! [3] //! [4]
{
    int rightbound = coords[0][0];
    for (int i = 0; i < 4; i++)
    {
        if (coords[i][0] > rightbound)
        {
            rightbound = coords[i][0];
        }
    }
    return rightbound;
}
//! [4]

//! [5]
int TetrixPiece::getBottomBound() const
{
    int bottombound = coords[0][1];
        for (int i = 0; i < 4; i++)
        {
            if (coords[i][1]<bottombound)
            {
                bottombound = coords[i][1];
            }
        }
        return bottombound;
}

int TetrixPiece::getUpBound() const
//! [5] //! [6]
{
    int upbound = coords[0][1];
        for (int i = 0; i < 4; i++)
        {
            if (coords[i][1]<upbound)
            {
                upbound = coords[i][1];
            }
        }
        return upbound;
}
//! [6]

//! [7]
TetrixPiece TetrixPiece::leftRotate() const
{
    if (blockShape == OShape)
        return *this;

    TetrixPiece result;
    result.blockShape = blockShape;
    for (int i = 0; i < 4; ++i) {
        result.setX(i, -getY(i));
        result.setY(i, getX(i));
    }
    int leftbound = result.getLeftBound();
    int upbound = result.getUpBound();
    if(leftbound!=0){
        int xdlb = -leftbound;
        for(int i=0;i<4;i++){
            result.coords[i][0]+=xdlb;
        }
    }
    if(upbound!=0){
        qDebug()<<"upbound:"<<upbound;
        int xdub = -upbound;
        for(int i=0;i<4;i++){
            result.coords[i][1]+=xdub;
        }
    }
    qDebug()<<"旋转后坐标：";
    for(int i=0;i<4;i++){
        qDebug()<<result.coords[i][0]<<result.coords[i][1];
    }
//! [7]
    return result;
}

//! [9]
TetrixPiece TetrixPiece::rightRotate() const
{
    if (blockShape == OShape)
        return *this;

    TetrixPiece result;
    result.blockShape = blockShape;
    for (int i = 0; i < 4; ++i) {
        result.setX(i, getY(i));
        result.setY(i, getX(i));
    }
//! [9]
    return result;
}

void TetrixPiece::setX(int index, int x) { coords[index][0] = x; }

void TetrixPiece::setY(int index, int y) { coords[index][1] = y; }
