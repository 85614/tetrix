#include"game.h"
using namespace std;


Game::Game(TetrixBoard *t)
{
    this->tboard = t;
}

void Game::moveLeft()
{
    tboard->tryMoveLeft();
}

void Game::moveRight()
{
    tboard->tryMoveRight();
}

void Game::rotatedLeft()
{
    tboard->rotatedLeft();
}

void Game::rotatedRight()
{
    tboard->rotatedRight();
}

void Game::oneLineDown()
{
    tboard->oneLineDownFirst();
}

void Game::dropDown()
{
    tboard->dropDown();
}

TetrixBoard::Level Game::level() const
{
    return tboard->level();
}

TetrixBoard::Level Game::level(TetrixBoard::Level newlevel)
{
    return tboard->level(newlevel);
}

int Game::getScore()
{
    return tboard->getScore();
}

TetrixPiece Game::getCurTetrixPiece()
{
    return tboard->getCurPiece();
}

bool Game::isFilled(int i, int j)
{
    if(tboard->isFilled(i,j))
        return true;
    else return false;
}


