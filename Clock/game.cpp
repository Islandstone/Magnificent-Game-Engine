#include "game.h"

#include "base.h"
#include "engine.h"
#include "gameinterface.h"
#include "camera.h"
#include "sprite.h"
#include "input.h"
#include "clock.h"

CGame_Factory CGame_FactoryObject;

CClock *pClock = NULL;

bool CGame::Init()
{
    Engine()->Debug(L"Loaded game Physics Test\n");

    pClock = new CClock();

    return true;
}

void CGame::PreStart()
{

    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);
    Engine()->GetDevice()->SetTransform( D3DTS_WORLD, &mat);

    /*
    g_pCamera->SetPosition(0,0);
    g_pCamera->Update();
    */

    pClock->Init();
}

void CGame::Start()
{
}

void CGame::Reset()
{
}

void CGame::Think()
{ 
    pClock->Think();
}

void CGame::Render()
{
    // TODO: Move this to some OnDeviceReset callback
    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);
    Engine()->GetDevice()->SetTransform( D3DTS_WORLD, &mat);

    g_pCamera->SetPosition(0,0);
    g_pCamera->Update();

    pClock->Render();
}

void CGame::Destroy()
{
    pClock->Destroy();
}

