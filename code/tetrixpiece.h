
#ifndef TETRIXPIECE_H
#define TETRIXPIECE_H

enum BlockShape { NoShape,LineShape,LShape,TShape,ZShape,OShape };

//! [0]
class TetrixPiece
{
public:
    TetrixPiece() { setShape(NoShape); }

    void setRandomShape();
    void setShape(BlockShape shape);

    BlockShape shape() const { return blockShape; }
    //得到某一行的X或Y坐标
    int getX(int index) const;
    int getY(int index) const;
    //得到方块的上下左右四个边界
    int getLeftBound() const;
    int getRightBound() const;
    int getBottomBound() const;
    int getUpBound() const;
    //方块的左旋和右旋
    TetrixPiece leftRotate() const;
    TetrixPiece rightRotate() const;
    //获取方块目前所在的位置的坐标
    int& siteX(){return curX;}
    int& siteY(){return curY;}
private:
    //更改第某个坐标的点X或Y坐标
    void setX(int index, int x);
    void setY(int index, int y);

    BlockShape blockShape;
    int coords[4][2];
    int curX;
    int curY;
};
//! [0]

#endif
