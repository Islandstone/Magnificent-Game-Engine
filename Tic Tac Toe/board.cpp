#include "board.h"

CBoard::CBoard()
{
    Reset();
}

void CBoard::Init()
{
    m_pBoard = new CSprite(L"textures/board.bmp");
    m_pO = new CSprite(L"textures/icon_o.bmp");
    m_pX = new CSprite(L"textures/icon_x.bmp");
}

void CBoard::Reset()
{
    ZeroMemory(m_iMatrix, 9*sizeof(int) );
    m_iNumberOfIcons = 0;
    m_bGameOver = false;
}

bool CBoard::CheckForDraw()
{
    if (m_iNumberOfIcons == 9)
    {
        m_bGameOver = true;
        return true;
    }

    return false;
}

bool CBoard::CheckForWin()
{
    // Check each column
    for (int x = 0; x < 3; x++)
    {
        int num_x = 0;
        int num_o = 0;

        for (int y = 0; y < 3; y++)
        {
            if (m_iMatrix[x][y] == PLAYER_X)
            {
                num_x += 1;
            }
            else if (m_iMatrix[x][y] == PLAYER_O)
            {
                num_o += 1;
            }
        }

        if (num_x == 3)
        {
            m_iWinner = PLAYER_X;
            m_bGameOver = true;
            return true;
        }

        if (num_o == 3)
        {
            m_iWinner = PLAYER_O;
            m_bGameOver = true;
            return true;
        }
    }

    // Check each row (reversed of the above)
    for (int x = 0; x < 3; x++)
    {
        int num_x = 0;
        int num_o = 0;

        for (int y = 0; y < 3; y++)
        {
            if (m_iMatrix[y][x] == PLAYER_X)
            {
                num_x += 1;
            }
            else if (m_iMatrix[y][x] == PLAYER_O)
            {
                num_o += 1;
            }
        }

        if (num_x == 3)
        {
            m_iWinner = PLAYER_X;
            m_bGameOver = true;
            return true;
        }

        if (num_o == 3)
        {
            m_iWinner = PLAYER_O;
            m_bGameOver = true;
            return true;
        }
    }
    
    // Check diagonal lines
    if ( m_iMatrix[1][1] != PLAYER_NONE )
    {
        int center = m_iMatrix[1][1];

        if ( m_iMatrix[0][0] == center && m_iMatrix[2][2] == center)
        {
            m_iWinner = center;
            m_bGameOver = true;
            return true;
        }

        if ( m_iMatrix[0][2] == center && m_iMatrix[2][0] == center )
        {
            m_iWinner = center;
            m_bGameOver = true;
            return true;
        }
    }

    return false;
}

// Return true if move was played
bool CBoard::PlayMove(int x, int y, int player)
{
    if (!IsTileFree(x, y))
    {
        return false;
    }

    PlaceMarker(x, y, player);
    m_iNumberOfIcons += 1;
    return true;
}

void CBoard::Update()
{
}

#define DELTA_WIDTH 160.0f
#define DELTA_HEIGHT 160.0f
#define SCALE 0.925f

void CBoard::Render()
{
    if (m_pBoard)
    {
        m_pBoard->Render();
    }

    for (int i = 0; i < 9; i++)
    {
        int x = i / 3;
        int y = i % 3;

        if ( IsTileFree( x, y ) )
        {
            continue;
        }

        CSprite *pIcon = m_iMatrix[x][y] == PLAYER_X ? m_pX : m_pO;

        if (!pIcon)
        {
            continue;
        }

        pIcon->SetPosition( (x-1)*DELTA_WIDTH, (y-1)*DELTA_HEIGHT );
        pIcon->SetScale( SCALE );

        pIcon->Render();
    }
}