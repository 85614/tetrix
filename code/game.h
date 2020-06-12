#ifndef GAME_H
#define GAME_H

#include"tetrixpiece.h"
#include"tetrixboard.h"
#include"QTextStream"
#include"QFile"

class Game{
public:
    enum Level{easy,normal,challenge};

    void start(){
        tboard->start();
    }
    void pause(){
        tboard->pause();
    }
    //或许可以有回退功能，方便AI的测试

    void moveLeft(){

    }
    void moveRight();

    void rotatedLeft();
    void rotatedRight();

    void oneLineDown();
    void dropDown();

    Level level()const;
    Level level(Level newlevel);

    int getScore();

    TetrixPiece getCurTetrixPiece();

    bool get(int i,int j);//判断目前某一个方块是否被填充

private:
    TetrixBoard *tboard;

};

#endif // GAME_H
