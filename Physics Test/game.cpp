#include "game.h"

#include "base.h"
#include "engine.h"
#include "gameinterface.h"
#include "camera.h"
#include "sprite.h"
#include "input.h"

#include "physics.h"

CGame_Factory CGame_FactoryObject;

bool CGame::Init()
{
    Engine()->Debug(L"Loaded game Physics Test\n");
    CPhysics *pPhysics = new CPhysics();
    Engine()->AddGameSystem( pPhysics );
    pPhysics->Init();

    return true;
}

void CGame::PreStart()
{
    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);
    Engine()->GetDevice()->SetTransform( D3DTS_WORLD, &mat);

    g_pCamera->SetPosition(0,0);
    g_pCamera->Update();
}

void CGame::Start()
{
}

void CGame::Reset()
{
}

void CGame::Think()
{ 
}

void CGame::Render()
{
    // TODO: Move this to some OnDeviceReset callback
    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);
    Engine()->GetDevice()->SetTransform( D3DTS_WORLD, &mat);

    g_pCamera->SetPosition(0,0);
    g_pCamera->Update();
}

void CGame::Destroy()
{
}

