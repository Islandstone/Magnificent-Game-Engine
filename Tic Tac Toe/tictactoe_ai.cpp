#include "tictactoe_ai.h"
#include <vector>
#include "engine.h"

inline String GetIconForPlayer(int player)
{
    if (player == PLAYER_X)
    {
        return L"X";
    }

    if (player == PLAYER_O)
    {
        return L"O";
    }

    return L" ";
}

void CTicTacToeAI::Reset()
{
    ZeroMemory( m_iWeightMap, 9*sizeof(int) );
}

int CTicTacToeAI::CalculateWeight(int x, int y)
{
    return CalculateForPlayer(x, y, PLAYER_X, 10) + CalculateForPlayer(x, y, PLAYER_O, 20);
}

int CTicTacToeAI::CalculateForPlayer(int x, int y, int player, int bias)
{
    int row = 0;
    int column = 0;
    int diagonal = 0;

    for (int i = 0; i < 3; i++)
    {
        if ( m_pBoard->GetTile(i, y) == player )
            row += 1;

        if ( m_pBoard->GetTile(x, i) == player )
            column += 1;
    }

    if (x == y && x == 1)
    {
        if (m_pBoard->GetTile(0,0) == player)
            diagonal += 1;

        if (m_pBoard->GetTile(0,2) == player)
            diagonal += 1;

        if (m_pBoard->GetTile(2,0) == player)
            diagonal += 1;

        if (m_pBoard->GetTile(2,2) == player)
            diagonal += 1;
    }
    else if (x == y && x == 0)
    {
        if (m_pBoard->GetTile(1,1) == player)
            diagonal += 1;

        if (m_pBoard->GetTile(2,2) == player)
            diagonal += 1;
    }
    else if (x == y && x == 2)
    {
        if (m_pBoard->GetTile(1,1) == player)
            diagonal += 1;

        if (m_pBoard->GetTile(0,0) == player)
            diagonal += 1;
    }
    else if (x == 2 && y == 0)
    {
        if (m_pBoard->GetTile(0,2) == player)
            diagonal += 1;

        if (m_pBoard->GetTile(1,1) == player)
            diagonal += 1;
    }
    else if (x == 0 && y == 2)
    {
        if (m_pBoard->GetTile(2,0) == player)
            diagonal += 1;

        if (m_pBoard->GetTile(1,1) == player)
            diagonal += 1;
    }

    if (row >= 2 || column >= 2 || diagonal >= 2)
    {
        return bias;
    }

    return row+column+diagonal;
}

struct Move_t
{
    int x;
    int y;
};

void CTicTacToeAI::DoMove( CBoard *pBoard )
{
    if (pBoard == NULL)
    {
        Engine()->FatalError(L"Error processing board AI");
    }

    Reset();

    m_pBoard = pBoard;

    int max = -1;
    std::vector<Move_t> choices;

    // Calculate weights on the board
    for (int i = 0; i < 9; i++)
    {
        int x = i / 3;
        int y = i % 3;

        if ( !pBoard->IsTileFree(x, y) )
        {
            continue;
        }

        m_iWeightMap[x][y] = CalculateWeight(x, y);

        if (m_iWeightMap[x][y] > max)
        {
            max = m_iWeightMap[x][y];

            choices.clear();

            Move_t move; 
            move.x = x;
            move.y = y;

            choices.push_back( move );
        }
        else if (m_iWeightMap[x][y] == max)
        {
            Move_t move; 
            move.x = x;
            move.y = y;

            choices.push_back( move );
        }
    }

    if (choices.size() == 0) // Zero division
    {
        return;
    }

    int n = rand() % choices.size();

    pBoard->PlayMove( choices[n].x, choices[n].y, PLAYER_O );
}