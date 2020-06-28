#ifndef AI_H
#define AI_H
#include<game.h>
#include<QBasicTimer>

class TetrixBoard;


class AI:public QObject {
private:
    TetrixBoard *tboard;

    //当前方块
    TetrixPiece curPes;
    //用4*4的数组记录当前方块的形状
    int shape[4][4];
    //当前局势
    int board[20][10];
    //得到的最佳落点{旋转次数，落点坐标x, 落点坐标y}
    int result[3];
public:
    Game *game;
    AI() {};
    AI(Game &game);
    //将目前的方块旋转90度
    void rotate();
    //获得最佳的落点信息
    void solve();
    int hole();
    double height();
    int erod();
    int row_translate();
    int col_translate();
    int space();
    int wells();
    double PD();
    int parameter;
    bool finishonepiece = true;
public slots:
    void operate();
};

#endif // AI_H
