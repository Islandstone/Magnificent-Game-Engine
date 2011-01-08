#ifndef TICTACTOE_AI_H
#define TICTACTOE_AI_H

#include "board.h"

class CTicTacToeAI
{
public:

    void DoMove( CBoard *pBoard );

protected:

    void Reset();

    int CalculateWeight(int x, int y);
    int CalculateForPlayer(int x, int y, int player, int bias);

private:
    CBoard          *m_pBoard;
    int             m_iWeightMap[3][3];
};

#endif // TICTACTOE_AI_H