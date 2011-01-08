#include "base.h"
#include "engine.h"
#include "gameinterface.h"
#include "game.h"
#include "camera.h"
#include "sprite.h"
#include "input.h"

CGame_Factory CGame_FactoryObject;

bool CGame::Init()
{
    Engine()->Debug(L"Loaded game Tic Tac Toe\n");
    m_pBoard = new CBoard();
    m_pAI = new CTicTacToeAI();

    m_iCurrentPlayer = PLAYER_X;

    return true;
}

void CGame::PreStart()
{
    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);
    Engine()->GetDevice()->SetTransform( D3DTS_WORLD, &mat);

    if (m_pBoard)
    {
        m_pBoard->Init();
    }

    g_pCamera->SetPosition(0,0);
    g_pCamera->Update();
}

void CGame::Start()
{
    m_flLastThinkTime = g_pGlobalTimer->CurrentTime();
}

void CGame::Reset()
{
    /*
    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);
    Engine()->GetDevice()->SetTransform( D3DTS_WORLD, &mat);

    g_pCamera->SetPosition(0,0);
    g_pCamera->Update();
    */

    m_pBoard->Reset();
    m_iCurrentPlayer = PLAYER_X;
}

void CGame::PlayAI()
{
    // TODO: Add some delay

    if (m_pAI)
    {
        m_pAI->DoMove(m_pBoard);

        m_iCurrentPlayer = PLAYER_X;
    }
}

struct TicTacToeInput_t 
{
    int keycode;
    int x;
    int y;
};

TicTacToeInput_t inputArray[] = {
    { VK_Q, 0, 0 },
    { VK_W, 1, 0 },
    { VK_E, 2, 0 },
    { VK_A, 0, 1 },
    { VK_S, 1, 1 },
    { VK_D, 2, 1 },
    { VK_Z, 0, 2 },
    { VK_X, 1, 2 },
    { VK_C, 2, 2 },
};

void CGame::PlayHuman()
{
    for (int i = 0; i < 9; i++)
    {
        TicTacToeInput_t input = inputArray[i];

        if ( g_pInput->KeyReleased( input.keycode ) )
        {
            bool success = m_pBoard->PlayMove(input.x, input.y, PLAYER_X);

            if (success)
            {
                m_iCurrentPlayer = PLAYER_O;
            }

            return;
        }
    }
}

void CGame::Think()
{ 
    if ( m_pBoard->IsGameOver() )
    {
        if ( g_pInput->KeyReleased( VK_F5 ) )
        {
            Reset();
        }

        return;
    }
    else
    {
        if ( !m_pBoard->CheckForWin() )
        {
            if (m_pBoard->CheckForDraw())
            {
                return;
            }
        }
        else
        {
            return;
        }
    }

    // Get current player
    // If player, check buttons
    // If AI, let the AI think
    // Attempt to commit the move
    // If successful
    // Check for win
    // Check for draw
    // Switch player

    if (m_iCurrentPlayer == PLAYER_X)
    {
        PlayHuman();
    }
    else if (m_iCurrentPlayer == PLAYER_O)
    {
        PlayAI();
    } 
}

void CGame::Render()
{
    // TODO: Move this to some OnDeviceReset callback
    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);
    Engine()->GetDevice()->SetTransform( D3DTS_WORLD, &mat);

    g_pCamera->SetPosition(0,0);
    g_pCamera->Update();

    if (m_pBoard)
    {
        m_pBoard->Render();
    }
}

void CGame::Destroy()
{
}

