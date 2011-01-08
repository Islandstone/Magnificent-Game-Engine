#ifndef BOARD_H
#define BOARD_H

#include "sprite.h"

enum
{
    PLAYER_NONE = 0,
    PLAYER_X,
    PLAYER_O,
};

enum 
{
    DRAW = 0,
    X_IS_WINNER,
    O_IS_WINNER,
};

class CBoard 
{
public:
    CBoard();

    void Init();

    bool PlayMove(int x, int y, int player);

    inline bool IsTileFree(int x, int y) { return m_iMatrix[x][y] == PLAYER_NONE; }
    inline void PlaceMarker(int x, int y, int player) { m_iMatrix[x][y] = player; }

    void Update();
    void Render();

    inline bool IsGameOver() { return m_bGameOver; }
    inline int GetWinner() { m_iWinner; }

    //int[3][3] GetMatrix() { return m_iMatrix; }
    inline int GetTile(int x, int y) { return m_iMatrix[x][y]; }

    inline void Reset();

    bool CheckForDraw();
    bool CheckForWin();

protected:

private:
    int                 m_iMatrix[3][3];
    int                 m_iWinner;
    bool                m_bGameOver;
    int                 m_iNumberOfIcons;

    CSprite             *m_pBoard;
    CSprite             *m_pO;
    CSprite             *m_pX;
};

#endif // BOARD_H