#include"ai.h"
#include<string>
#include<game.h>
#include<qdebug.h>
#include<string.h>
using namespace std;

#define BoardWidth 10
#define BoardHeight 20

int s = 0;


AI::AI(Game& game){
    this->game = &game;
    curPes = game.getCurTetrixPiece();
    memset(board,0,sizeof(board));
    memset(shape,0,sizeof(shape));

}


void AI::operate(){

    qDebug()<<"调用";
    solve();
}
void AI::rotate() {
    curPes=curPes.leftRotate();
    memset(shape, 0, sizeof(shape));
    for (int i = 0;i < 4;i++) {
        int x = curPes.getX(i);
        int y = curPes.getY(i);
        shape[y][x] = 1;
    }
}



void AI::solve() {
    if(!finishonepiece){
        return;
    }

    memset(board,0,sizeof(board));
    memset(shape,0,sizeof(shape));
    curPes = game->getCurTetrixPiece();

    if(curPes.shape()==NoShape){
        return;
    }
    qDebug()<<"调用成功";
    qDebug()<<curPes.shape();
    for(int i=0;i<BoardHeight;i++){
        for(int j=0;j<BoardWidth;j++){
            board[i][j]=game->isFilled(BoardHeight-i-1,j);

        }
    }


    finishonepiece = false;
    for (int i = 0;i < 4;i++) {
        int x = curPes.getX(i);
        int y = curPes.getY(i);
//        qDebug()<<"getX"<<x;
//        qDebug()<<"getY"<<y;
        shape[y][x] = 1;
    }

    for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
        qDebug()<<shape[i][j];
    }
    qDebug()<<endl;
    }
//    for(int i=0;i<20;i++){
//    for(int j=0;j<10;j++){
//        qDebug()<<board[i][j]<<" ";
//    }
//    qDebug()<<endl;
//    }

    double max = -(1 << 23);
    int result[2];
    for (int r = 0;r < 4;r++) {
        int rightBound = curPes.getRightBound();
        qDebug()<<"r:"<<rightBound;
        int bottomBound = curPes.getBottomBound();
        qDebug()<<"b:"<<bottomBound;
        for (int i = 0;i < BoardWidth - rightBound;i++) {
            for (int j = 0;j < BoardHeight - bottomBound;j++) {
                //判断(i,j)是否可以填充
                bool flag = 1;
                for (int s = 0;s <= rightBound;s++) {
                    for (int k = 0;k <= bottomBound;k++) {
                        if (board[j + k][i + s] && shape[k][s]) {
                            flag = 0;
                            break;
                        }
                    }
                }
                //不能填充的话则(i,j-1)为落点
                if (!flag||j==(BoardHeight-bottomBound-1)) {
                    //更改board，新落下的格子填充2
                    if(j==BoardHeight-bottomBound-1){
                                         j++;
                         }
                    for (int s = 0;s <= rightBound;s++) {
                        for (int k = 0;k <= bottomBound;k++) {
                            if (shape[k][s]) {
                                board[j + k - 1][i + s] = 2;
                            }
                        }
                    }

                    double score = PD();
                    qDebug()<<"score"<<score;
                    if (max < score) {
                        for(int i=0;i<BoardHeight;i++){
                            qDebug()<<board[i][0]<<board[i][1]<<board[i][2]<<board[i][3]<<board[i][4]<<board[i][5]<<board[i][6]<<board[i][7]<<board[i][8]<<board[i][9];
                        }
                        qDebug()<<"max";
                        max = score;
                        result[0] = r;
                        result[1] = i;
                    }
                    //回溯board
                        for(int i=0;i<BoardHeight;i++){
                            for(int j=0;j<BoardWidth;j++){
                                board[i][j]=game->isFilled(BoardHeight-i-1,j);

                            }
                        }

                    break;
                }
            }
        }
        //枚举方块旋转90度的情况

        rotate();
    }
     qDebug()<<"旋转次数"<<result[0];
     qDebug()<<"坐标"<<result[1];

    for(int i=0;i<result[0];i++){
        game->rotatedLeft();
        qDebug()<<"rotate";
    }
    int moveNum=result[1]-5;
    if(moveNum>=0){
    for(int i=0;i<moveNum;i++){
        game->moveRight();
        qDebug()<<"L1";
    }
    }
    else{
        for(int i=moveNum;i<0;i++){
            game->moveLeft();
            qDebug()<<"R1";
        }
    }
    game->dropDown();

    finishonepiece = true;
}





double AI::height() {
    double h_low, h_high, h;
    int k = 0;//方块的低点，高点；k用来判断是否已经给出h_high的值，若为1则已给出。
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j <= 9; j++) {
            if (board[i][j] == 2) {
                h_high = 19 - i;
                k++;
                break;
            }//给h_high赋值
        }
        if (k == 1) {
            break;
        }//给h_high赋值后，退出由上至下的循环。
    }
    k = 0;
    for (int i = 19; i >= 0; i--) {
        for (int j = 0; j < 10; j++) {
            if (board[i][j] == 2) {
                h_low = 19 - i;
                k++;
            }
        }
        if (k == 1) {
            break;
        }//给h_low赋值后，退出由下至上的循环。
    }
    h = (h_high + h_low) / 2;
    return h;
}//方块的高度参数，消除前。
int AI::erod() {
    int u = 0, v = 0,w=0;//u是新贡献的方块个数，v是消除的行数；
    for (int i = 19; i >= 0; i--) {
        for (int j = 0; j <= 9; j++) {
            if (board[i][j] == 2) { u++; }//记录新贡献的方块的个数(无论本行能否被消除)。
            if (board[i][j] == 0) {
                u = 0;//若本行不能被消除，则记录的新贡献的方块个数归零。
                break;
            }
            if (j == 9 && board[i][j] != 0) { v++; w = w + u * v; }//v是消除的行数，w是总的erod值。

        }
    }
    return w;
}//有关消除的参数

int AI::row_translate() {
    int row = 0;
    for (int i = 19; i >= 0; i--) {
        for (int j = 0; j <= 9; j++) {
            if (j == 9 && board[i][j] == 0) {
                row++;
            }
            if (j <= 9 && j>0) {
                if (board[i][j] != board[i][j - 1]) {
                    row++;
                }
            }
        }
    }
    return row;
}//行变换参数,消除前。

int AI::col_translate() {
    int col = 0;
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 20; i++) {

            if (i==19 && board[i][j] == 0) {
                col++;
            }
            if (i <= 19 && i>0) {
                if (board[i][j] != board[i - 1][j]) {
                    col++;
                }
            }
        }
    }
    return col;
}//列变换参数，消除前。


int AI::hole() {
 int holes = 0;
  for (int i = 19; i >= 0; i--) {
  for (int j = 0; j <= 9; j++) {
   if (board[i][j] == 0) {
    break;
   }
   if (j == 9 && board[i][j]!=0) { board[i][0] = 3; }
  }
 }//将每个需要消除的行前面标3。
 for (int i = 0; i < BoardWidth; i++) {
  int forward = board[0][0] == 3 ? 0 : board[0][i];
  for (int j = 1; j < BoardHeight; j++) {
   if (board[j][0] == 3) {
    continue;
   }
   if (forward && !board[j][i]) {
    holes++;
   }
   forward = board[j][i];
  }
 }
 return holes;
}//空格之和

int AI::wells() {
    int WELL = 0, amount_left = 1, amount_middle = 1, amount_right = 1;
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 20; i++) {
            if (j == 0) {
                if (board[i][j] == 0 && board[i][1] != 0) {
                    WELL += amount_left;
                    amount_left++;
                }
                if(board[i+1][j]!=0 && i!=19) {
                    amount_left = 1;
                }
            }//靠左边
            if (j < 9 && j> 0) {
                if (board[i][j] == 0 && board[i][j - 1] != 0 && board[i][j + 1] != 0) {
                    WELL += amount_middle;
                    amount_middle++;
                }
                if(board[i+1][j]!=0 && i != 19) {
                    amount_middle = 1;
                }
            }//不靠边
            if (j == 9) {
                if (board[i][j] == 0 && board[i][j-1] != 0) {
                    WELL += amount_right;
                    amount_right++;
                }
                if (board[i + 1][j] != 0 && i != 19) {
                    amount_right = 1;
                }
            }//靠右边
        }
    }
    return WELL;
}//井字深度和
double AI::PD(){
    return 34 * erod() - 45 * height() - 32 * row_translate() - 93 * col_translate() - 34 * wells() - 79 * hole();
}
