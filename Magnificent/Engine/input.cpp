#include "base.h"
#include "input.h"
#include <windows.h>

CInput *g_pInput = NULL;

CInput::CInput()
{
    g_pInput = this;

    m_pCurrentState = m_szKeyBuffer1;
    m_pPreviousState = m_szKeyBuffer2;

    ZeroMemory( m_pCurrentState, sizeof(m_szKeyBuffer1) );
    ZeroMemory( m_pPreviousState, sizeof(m_szKeyBuffer2) );
}

void CInput::PostRender()
{
    for (int i = 0; i < 255; i++)
    {
        m_pPreviousState[i] = m_pCurrentState[i];
    }
}

void CInput::SetKeyUp(int keycode)
{
    m_pCurrentState[keycode] = 0;
}

void CInput::SetKeyDown(int keycode)
{
    m_pCurrentState[keycode] = 1;
}

bool CInput::KeyUp(int keycode)
{
    return (m_pCurrentState[keycode] == 0) /*&& (m_pPreviousState[keycode] == 0)*/;
}

bool CInput::KeyPressed(int keycode)
{
    return (m_pCurrentState[keycode] == 1) && (m_pPreviousState[keycode] == 0);
}

bool CInput::KeyHeld(int keycode)
{
    return (m_pCurrentState[keycode] == 1) /*&& (m_pPreviousState[keycode] == 0)*/;
}

bool CInput::KeyReleased(int keycode)
{
    return (m_pCurrentState[keycode] == 0) && (m_pPreviousState[keycode] == 1);
}