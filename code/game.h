#ifndef GAME_H
#define GAME_H

#include"tetrixpiece.h"
#include"tetrixboard.h"
#include"QTextStream"
#include"QFile"


//为ＡＩ组准备的最基础的功能接口　实现了最基本的一些操作功能
//ＡＩ组也可以根据自己的需求定义实现新的函数
class Game{
public:
    Game(TetrixBoard *tborad);
    enum Level{easy,normal,challenge};

    void start(){
        tboard->start();
    }
    void pause(){
        tboard->pause();
    }
    //或许可以有回退功能，方便AI的测试

    void moveLeft();
    void moveRight();

    void rotatedLeft();
    void rotatedRight();

    //最下方的方块向下坠落一格
    void oneLineDown();
    //最下方的方块直接落地
    void dropDown();

    //获取当前的难度 共分为easy normal hard三等
    TetrixBoard::Level level()const;
    //重新设置一个Level 返回值为旧的Level
    TetrixBoard::Level level(TetrixBoard::Level newlevel);

    int getScore();

    //返回目前正在操纵的方块对象
    TetrixPiece getCurTetrixPiece();

    bool isFilled(int i,int j);//判断目前某一个方块是否被填充

private:
    TetrixBoard *tboard;

};

#endif // GAME_H
