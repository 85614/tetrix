#include"tetrixboard.h"


TetrixPiece TetrixBoard::getNextPiece() const
{
     TetrixPiece ans;
     ans.setRandomShape();
     return ans;
}
