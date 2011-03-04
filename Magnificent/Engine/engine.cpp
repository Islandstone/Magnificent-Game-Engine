#include "base.h"

#include "Shiny.h"

#include "engine.h"
#include "dx_error_check.h"
#include "timer.h"
#include "input.h"
#include "camera.h"
#include "texture.h"

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

#include "Dxerr.h"
#pragma comment(lib, "Dxerr.lib" )

CEngine* g_sInstance = NULL;
bool     g_bActive = true;
bool     g_bPastInitDelay = false;
bool     g_bChangingWindow = false;

bool IsVistaOrWin7OrLater() {
    DWORD version = GetVersion();
    DWORD major = (DWORD) (LOBYTE(LOWORD(version)));
    DWORD minor = (DWORD) (HIBYTE(LOWORD(version)));

    return (major > 6) || ((major == 6) && (minor >= 0));
}


CEngine::CEngine()
{
    m_bFullscreen = false;
    m_bFailed = false;
    m_bGameStarted = false;

    m_iScreenX = 0;
    m_iScreenY = 0;

    m_iSplashCount = 0;

    m_pd3d = NULL;
    m_pd3ddev = NULL;

    m_bActive = true;

    m_hInstance = NULL;
    m_hwnd = NULL;  

    m_bSplashScreenDisabled = false;
    g_sInstance = this;
}

bool CEngine::Init( HINSTANCE hInstance )
{
    m_hInstance = hInstance;

    CreateSystems();
    
    if ( !InitSystems() )
    {
        Error(L"Failed to init game systems!");
        return false;
    }

    if ( g_pGameFactory != NULL )
    {
        m_pGame = g_pGameFactory->Create();
    }

    if (m_pGame != NULL)
    {
        m_pGame->Init();
    }

    WNDCLASSEX   wndclassex = {0};

    wndclassex.cbSize        = sizeof(WNDCLASSEX);
    wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc   = &WndProc;
    wndclassex.cbClsExtra    = 0;
    wndclassex.cbWndExtra    = 0;
    wndclassex.hInstance     = m_hInstance;
    wndclassex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndclassex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclassex.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclassex.lpszMenuName  = L"";
    wndclassex.lpszClassName = APPNAME;
    wndclassex.hIconSm       = wndclassex.hIcon;

    if ( !RegisterClassEx(&wndclassex) )
    {
        //MessageBox (NULL, TEXT ("RegisterClassEx failed!"), szAppName, MB_ICONERROR);
        Error( TEXT ("RegisterClassEx failed!") );
        return false;
    }

    if (IsScreenSaver())
    {
        
        int screen_x = GetSystemMetrics( SM_CXSCREEN );
        int screen_y = GetSystemMetrics( SM_CYSCREEN );
        bool fullscreen = true; //!IsVistaOrWin7OrLater();

        //Message(fullscreen ? L"Fullscreen" : L"Windowed" );

        if (!screen_x || !screen_y)
        {
            Error(L"Failed to enumerate screen size!");
            return false;
        }
        

        if ( !InitWindow(screen_x, screen_y, fullscreen ) )
        {
            Error(L"Failed to create window!");
            return false;
        }

        if ( !InitDirect3D(fullscreen) )
        {
            Error(L"Failed to init DirectX!");
            return false;
        }
    }
    else
    {
        if ( !InitWindow(SCREEN_SIZE_X, SCREEN_SIZE_Y, FULLSCREEN) )
        {
            Error(L"Failed to create window!");
            return false;
        }

        if ( !InitDirect3D(FULLSCREEN) )
        {
            Error(L"Failed to init DirectX!");
            return false;
        }
    }

    return true;
}

bool CEngine::InitWindow(int x, int y, bool fullscreen)
{
    DWORD style = fullscreen ? WS_POPUP : WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    m_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, 
                            APPNAME, 
                            TEXT ("Magnificent Game Engine"),
                            style,
                            CW_USEDEFAULT, 
                            CW_USEDEFAULT, 
                            x, 
                            y, 
                            NULL, 
                            NULL, 
                            m_hInstance,
                            NULL);

    if (m_hwnd == NULL)
    {
        Error(L"CreateWindow returned NULL pointer");
        return false;
    }

    ShowWindow(m_hwnd, 1);
    UpdateWindow(m_hwnd);

    // Get client rectangle
    GetClientRect(m_hwnd, &ClientRect);
    GetWindowRect(m_hwnd, &WindowRect);

    // Ensure proper size of the client area
    int delta_width    = x-(ClientRect.right - ClientRect.left);
    int delta_height   = y-(ClientRect.bottom - ClientRect.top);

    RECT newRect;

    SetRect(&newRect,
            WindowRect.left,
            WindowRect.top,
            WindowRect.right + delta_width,
            WindowRect.bottom + delta_height
        );

    AdjustWindowRect(&newRect, style, FALSE);

    // Get new client rectangle
    GetClientRect(m_hwnd, &ClientRect);
    GetWindowRect(m_hwnd, &WindowRect);

    m_iScreenX = x;
    m_iScreenY = y;

    return true;
}

void CEngine::EnumerateDevice()
{
    const D3DFORMAT fmts[] =    {      D3DFMT_A1R5G5B5,      D3DFMT_A2R10G10B10,      D3DFMT_A8R8G8B8,      D3DFMT_R5G6B5,      D3DFMT_X1R5G5B5,      D3DFMT_X8R8G8B8,   };   
    const int nBits[] = {16, 32, 32, 16, 16, 32};   
    const int nNumFormats = 6;   
    
    ScreenMode theMode;   
    
    m_vModes.clear();   
    
    for(int i=0; i<nNumFormats; ++i)   
    {      
        UINT nCount = m_pd3d->GetAdapterModeCount(D3DADAPTER_DEFAULT,fmts[i]);      
        
        for(UINT j=0; j<nCount; ++j)      
        {         
            if(SUCCEEDED(m_pd3d->EnumAdapterModes(D3DADAPTER_DEFAULT,fmts[i],j,&theMode.theMode)))         
            {            
                theMode.fmt = fmts[i];            
                theMode.nBits = nBits[i];            
                m_vModes.push_back(theMode);

            }     
        }   
    }
}

bool CEngine::InitDirect3D(bool fullscreen)
{
    if (m_pd3d == NULL)
    {
        if( NULL == ( m_pd3d = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        {
            Error(L"Failed to create D3D interface!");
            return false;
        }
    }

    //EnumerateDevice();

    D3DDISPLAYMODE d3ddm;

    m_pd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,
        &d3ddm);

    D3DPRESENT_PARAMETERS window_params;
    ZeroMemory( &window_params, sizeof(D3DPRESENT_PARAMETERS) );

    window_params.BackBufferWidth         = m_iScreenX;
    window_params.BackBufferHeight        = m_iScreenY;
    window_params.BackBufferFormat        = d3ddm.Format;
    window_params.BackBufferCount         = 1;

    window_params.MultiSampleType         = D3DMULTISAMPLE_8_SAMPLES;
    window_params.MultiSampleQuality      = 0;

    window_params.SwapEffect              = D3DSWAPEFFECT_DISCARD;
    window_params.hDeviceWindow           = m_hwnd;
    window_params.Windowed                = TRUE;
    window_params.EnableAutoDepthStencil  = FALSE;
    window_params.AutoDepthStencilFormat  = D3DFMT_D32;
    window_params.Flags = 0;

    window_params.FullScreen_RefreshRateInHz = 0; /* FullScreen_RefreshRateInHz must be zero for Windowed mode */
    window_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    D3DPRESENT_PARAMETERS full_params;
    ZeroMemory( &full_params, sizeof(D3DPRESENT_PARAMETERS) );

    full_params.BackBufferWidth         = m_iScreenX;
    full_params.BackBufferHeight        = m_iScreenY;
    full_params.BackBufferFormat        = D3DFMT_X8R8G8B8;
    full_params.BackBufferCount         = 1;

    full_params.MultiSampleType         = D3DMULTISAMPLE_8_SAMPLES;
    full_params.MultiSampleQuality      = 0;

    full_params.SwapEffect              = D3DSWAPEFFECT_DISCARD;
    full_params.hDeviceWindow           = m_hwnd;
    full_params.Windowed                = FALSE;
    full_params.EnableAutoDepthStencil  = FALSE;
    full_params.AutoDepthStencilFormat  = D3DFMT_D32;
    full_params.Flags = 0;

    full_params.FullScreen_RefreshRateInHz = 60;
    full_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    D3DPRESENT_PARAMETERS params = fullscreen ? full_params : window_params;

    DWORD qualityLevel;
    if( FAILED(m_pd3d->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
        D3DDEVTYPE_HAL, params.BackBufferFormat, 
        params.Windowed, params.MultiSampleType, &qualityLevel ) ) )
    {
        params.MultiSampleType      = D3DMULTISAMPLE_NONE;
        params.MultiSampleQuality   = 0;
    }
    else
    {
        params.MultiSampleQuality = (int)qualityLevel - 1;
    }

    HRESULT hres = m_pd3d->CreateDevice( D3DADAPTER_DEFAULT, 
        D3DDEVTYPE_HAL, 
        m_hwnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &params,
        &m_pd3ddev );

    if( FAILED( hres ) )
    {
        String msg = String(L"Failed to create D3D device! Error: ") + DXGetErrorString(hres);
        Error(msg);  

        return false;
    }

    // TODO: Extract this into a separate function SetDefaultSettings();
    if ( FAILED( m_pd3ddev->SetRenderState(D3DRS_LIGHTING, FALSE) ) || 
        FAILED( m_pd3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ) ||
        FAILED( m_pd3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA) ) ||
        FAILED( m_pd3ddev->SetRenderState(D3DRS_CULLMODE, TRUE) )
        )
    {
        Error(L"Failed to set initial render states!");
        return false;
    }

    if (!IsScreenSaver() && ShouldRenderSplashes() )
    {
        m_pLogoSprite = new CSprite(L"logo.bmp");
        m_pSplashSprite = new CSprite(L"splash.bmp");
    }
    
    return true;
}

bool CEngine::ResetDirect3D(bool fullscreen)
{
    if (m_pd3d == NULL)
    {
        Error(L"Failed to create D3D interface!");
        return false;
    }

    D3DDISPLAYMODE d3ddm;

    m_pd3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,
        &d3ddm);

    D3DPRESENT_PARAMETERS window_params;
    ZeroMemory( &window_params, sizeof(D3DPRESENT_PARAMETERS) );

    window_params.BackBufferWidth         = m_iScreenX;
    window_params.BackBufferHeight        = m_iScreenY;
    window_params.BackBufferFormat        = d3ddm.Format;
    window_params.BackBufferCount         = 1;

    window_params.MultiSampleType         = D3DMULTISAMPLE_8_SAMPLES; //D3DMULTISAMPLE_8_SAMPLES;
    window_params.MultiSampleQuality      = 0;

    window_params.SwapEffect              = D3DSWAPEFFECT_DISCARD;
    window_params.hDeviceWindow           = m_hwnd;
    window_params.Windowed                = TRUE;
    window_params.EnableAutoDepthStencil  = FALSE;
    window_params.AutoDepthStencilFormat  = D3DFMT_D32;
    window_params.Flags = 0;

    window_params.FullScreen_RefreshRateInHz = 0; // Always 0 for windowed mode
    window_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    D3DPRESENT_PARAMETERS full_params;
    ZeroMemory( &full_params, sizeof(D3DPRESENT_PARAMETERS) );

    full_params.BackBufferWidth         = m_iScreenX;
    full_params.BackBufferHeight        = m_iScreenY;
    full_params.BackBufferFormat        = d3ddm.Format;
    full_params.BackBufferCount         = 1;

    full_params.MultiSampleType         = D3DMULTISAMPLE_8_SAMPLES;
    full_params.MultiSampleQuality      = 0;

    full_params.SwapEffect              = D3DSWAPEFFECT_DISCARD;
    full_params.hDeviceWindow           = m_hwnd;
    full_params.Windowed                = FALSE;
    full_params.EnableAutoDepthStencil  = FALSE;
    full_params.AutoDepthStencilFormat  = D3DFMT_D32;
    full_params.Flags = 0;

    // TODO: Add enumeration for this option
    full_params.FullScreen_RefreshRateInHz = 60;
    full_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Select params based on fullscreen setting
    D3DPRESENT_PARAMETERS params = fullscreen ? full_params : window_params;

    // Set multisampling quality to highest setting if available
    DWORD qualityLevel;
    if( FAILED(m_pd3d->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
        D3DDEVTYPE_HAL, params.BackBufferFormat, 
        params.Windowed, params.MultiSampleType, &qualityLevel ) ) )
    {
        // Not available
        params.MultiSampleType      = D3DMULTISAMPLE_NONE;
        params.MultiSampleQuality   = 0;
    }
    else
    {
        // Available, max setting is qualityLevel-1
        params.MultiSampleQuality = (int)qualityLevel - 1;
    }

    OnLostDevice();
    HRESULT hres = m_pd3ddev->Reset( &params );
    OnResetDevice();

    if( FAILED( hres ) )
    {
        Error(L"Failed to reset D3D device!");
        return false;
    }

    if ( FAILED( m_pd3ddev->SetRenderState(D3DRS_LIGHTING, FALSE) ) || 
        FAILED( m_pd3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ) ||
        FAILED( m_pd3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA) ) ||
        FAILED( m_pd3ddev->SetRenderState(D3DRS_CULLMODE, TRUE) )
        )
    {
        Error(L"Failed to set initial render states!");
        return false;
    }

    return true;
}

void CEngine::CreateSystems()
{
    AddGameSystem( Sound() );
    AddGameSystem( Input() );
}

bool CEngine::InitSystems()
{
    // Update game systems
    for (unsigned int i = 0; i < m_vecGameSystems.size(); i++)
    {
        if (m_vecGameSystems[i] != NULL)
        {
            if ( !m_vecGameSystems[i]->Init() )
            {
                Error( String(L"Failed to init ") + m_vecGameSystems[i]->GetName() );
                return false;
            }
        }
    }

    return true;
}

void CEngine::Start()
{
    if (m_pGame)
    {
        m_pGame->PreStart();
    }

    Timer()->Start();

    m_tProfileTimer.Start();

    Main();
}

void CEngine::Main()
{
    MSG msg;

    ZeroMemory(&msg, sizeof (msg));

    while ( msg.message != WM_QUIT )
    {
        // Check for messages
        if (PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            if ( !g_bActive )
            {
                WaitMessage();
                continue;
            }

            PROFILE_BEGIN( Frame );
            FrameAdvance();
            PROFILE_END();

            PROFILER_UPDATE();

            if (m_tProfileTimer.CurrentTime() >= PROFILE_OUTPUT_FREQ)
            {
                OutputDebugStringA( PROFILER_OUTPUT_TREE_STRING().c_str() );
                //OutputDebugStringA( PROFILER_OUTPUT_FLAT_STRING().c_str() );
                OutputDebugStringA("\n");
                m_tProfileTimer.Reset();
                m_tProfileTimer.Start();
            }


            // Try not to kill the computer we're running on
            // TODO: Add a proper time burn
            Sleep(10);
        }

        Sleep(0);
    }
}

void CEngine::Stop()
{
    Destroy();
}

void CEngine::FrameAdvance()
{
    PROFILE_BEGIN( GameSystemsUpdate );
    // Update game systems
    for (unsigned int i = 0; i < m_vecGameSystems.size(); i++)
    {
        if (m_vecGameSystems[i] != NULL)
        {
            m_vecGameSystems[i]->Update();
        }
    }
    PROFILE_END();

    if ( ShouldRenderSplashes() )
    {
        // Using a delay timer so the engine doesn't pop when startin
        if (g_bPastInitDelay)
        {
            PROFILE_BEGIN( PreRender );
            PreRender();
            PROFILE_END();
            if (IsActive())
            {
                PROFILE_BEGIN( RenderSplashScreens );
                RenderSplashScreens();
                PROFILE_END();
            }
            PROFILE_BEGIN( PostRender );
            PostRender();
            PROFILE_END();
        }
        else
        {
            if (!m_tFadeTimer.IsStarted())
            {
                m_tFadeTimer.Start();
            }
            else if (m_tFadeTimer.CurrentTime() >= INIT_DELAY)
            {
                m_tFadeTimer.Reset();
                g_bPastInitDelay = true;
            }
        }
        return;
    }
    else if (m_pGame != NULL && !m_bGameStarted)
    {
        m_bGameStarted = true;
        m_pGame->Start();
    }

    if (m_pGame == NULL)
    {
        return;
    }

    PROFILE_BEGIN( GameThinkRoot );
        PROFILE_BEGIN( GamePreThink );
            m_pGame->PreThink();
        PROFILE_END();
        PROFILE_BEGIN( GameThink );
            m_pGame->Think();
        PROFILE_END();
        PROFILE_BEGIN( GamePostThink );
            m_pGame->PostThink();
        PROFILE_END();
    PROFILE_END();

    if ( Input()->KeyReleased( VK_F2 ) )
    {
        ChangeWindow();
    }

    PROFILE_BEGIN( PreRenderRoot );
    
    PROFILE_BEGIN( EnginePreRender );
    PreRender();
    PROFILE_END();

    PROFILE_BEGIN( GamePreRender );
    m_pGame->PreRender();
    PROFILE_END();

    PROFILE_END();

    PROFILE_BEGIN( RenderRoot );

    PROFILE_BEGIN( GameRender );
    m_pGame->Render();
    PROFILE_END();

    PROFILE_BEGIN( GameSystemsRender );
    // Render game systems
    for (unsigned int i = 0; i < m_vecGameSystems.size(); i++)
    {
        if (m_vecGameSystems[i] != NULL)
        {
            m_vecGameSystems[i]->Render();
        }
    }
    PROFILE_END(); // GameSystemsRender

    PROFILE_END(); // Render root

    PROFILE_BEGIN( PostRenderRoot );
    PROFILE_BEGIN( GamePostRender );
    m_pGame->PostRender();
    PROFILE_END();
    PROFILE_BEGIN( EnginePostRender );
    PostRender();
    PROFILE_END();
    
    PROFILE_BEGIN( GameSystemsPostRender );
    // PostRender game systems
    for (unsigned int i = 0; i < m_vecGameSystems.size(); i++)
    {
        if (m_vecGameSystems[i] != NULL)
        {
            m_vecGameSystems[i]->PostRender();
        }
    }
    PROFILE_END();

    PROFILE_END(); // Post render root
}

void CEngine::PreRender()
{
    if (m_pd3ddev == NULL )
    {
        return;
    }

    D3D_CHECK_BEGIN( m_pd3ddev->BeginScene() ); 
        D3D_CHECK( D3DERR_INVALIDCALL, L"IDirect3DDevice::BeginScene() called without first calling IDirect3DDevice::EndScene()!" )
    D3D_CHECK_END();   

    D3D_CHECK_BEGIN( m_pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 ) );
        D3D_CHECK( D3DERR_INVALIDCALL, L"Invalid call to IDirect3DDevice::Clear()!" );
    D3D_CHECK_END();
}

void CEngine::PostRender()
{
    if (m_pd3ddev == NULL)
    {
        return;
    }

    D3D_CHECK_BEGIN( m_pd3ddev->EndScene() );
        D3D_CHECK( D3DERR_INVALIDCALL, L"IDirect3DDevice::EndScene() called without first calling IDirect3DDevice::BeginScene()" );
    D3D_CHECK_END();

    // TODO: Optimize dirty region?
    D3D_CHECK_BEGIN( m_pd3ddev->Present(NULL, NULL, NULL, NULL) )
        D3D_CHECK( D3DERR_DEVICEREMOVED, L"Device was removed before calling IDirect3DDevice::Present()!");

    /*
        // Bit of a hack
        if (_hResult == D3DERR_DEVICELOST)
        {
            //OnLostDevice();
        }
        */

    D3D_CHECK_END();
}

void CEngine::ChangeWindow()
{
    SetActive(false);
    g_bChangingWindow = true;

    if (m_bFullscreen)
    {
        // Set windowed mode
        SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);

        m_iScreenX = SCREEN_X_WIN;
        m_iScreenY = SCREEN_Y_WIN;

        if (ResetDirect3D(false))
        {
            m_bFullscreen = false;

            // Set the window position
            SetWindowPos(m_hwnd, HWND_NOTOPMOST,
                WindowRect.left, WindowRect.top,
                (WindowRect.right - WindowRect.left),
                (WindowRect.bottom - WindowRect.top),
                SWP_SHOWWINDOW);
        }
        else
        {
            // Failed
            return;
        }
    }
    else
    {
        // Set fullscreen mode
        SetWindowLong(m_hwnd, GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE);

        m_iScreenX = SCREEN_X_FULL;
        m_iScreenY = SCREEN_Y_FULL;

        if ( ResetDirect3D(true) )
        {
            m_bFullscreen = true;
        }
        else
        {
            // Failed, change back
            m_iScreenX = SCREEN_X_WIN;
            m_iScreenY = SCREEN_Y_WIN;

            // Set the window position
            SetWindowPos(m_hwnd, HWND_NOTOPMOST,
                WindowRect.left, WindowRect.top,
                (WindowRect.right - WindowRect.left),
                (WindowRect.bottom - WindowRect.top),
                SWP_SHOWWINDOW);
        }
    }

    SetActive(true);

    // Get new client rectangle
    GetClientRect(m_hwnd, &ClientRect);

    g_bChangingWindow = false;
}

LRESULT CALLBACK CEngine::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        return 0;

    case WM_ACTIVATE:
        {
            if (g_bChangingWindow)
            {
                return 0;
            }

            bool newState = !HIWORD(wParam);
            //newState ? Engine()->OnResetDevice() : Engine()->OnLostDevice();
            //Engine()->SetActive(newState);
            g_bActive = newState;
            return 0;
        }
        
#ifndef SCREENSAVER
    case WM_KEYDOWN:
        if ((lParam & 1<<30) == 0)
        {
            Input()->SetKeyDown(wParam);
        }

        return (0);

    case WM_KEYUP:
        Input()->SetKeyUp(wParam);

        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }

        return (0);
#else
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_MOUSEMOVE:
        PostQuitMessage(0);
        return (0);
#endif

    case WM_CLOSE:
    case WM_DESTROY:
        if (!g_bChangingWindow)
        {
            PostQuitMessage (0);
        }

        return (0);

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

void CEngine::OnLostDevice()
{
    Debug(L"Device lost!\n");

    SetActive(false);

    for (unsigned int i = 0; i < m_vecObjectList.size(); i++)
    {
        HRESULT res = S_OK;
        IUnknown *pUnknown = m_vecObjectList[i];
        
        if ( pUnknown != NULL)
        {
            void *pObject = NULL;

            res = pUnknown->QueryInterface(IID_ID3DXSprite, &pObject);

            if (SUCCEEDED(res) && pObject != NULL)
            {
                ID3DXSprite *pSprite = (ID3DXSprite *)pObject;
                pSprite->OnLostDevice();
                pSprite->Release();
                continue;
            }

            res = pUnknown->QueryInterface(IID_ID3DXLine, &pObject);

            if (SUCCEEDED(res) && pObject != NULL)
            {
                ID3DXLine *pLine = (ID3DXLine *)pObject;
                pLine->OnLostDevice();
                pLine->Release();
                continue;
            }

            res = pUnknown->QueryInterface(IID_ID3DXFont, &pObject);

            if (SUCCEEDED(res) && pObject != NULL)
            {
                ID3DXFont *pFont = (ID3DXFont *)pObject;
                pFont->OnLostDevice();
                pFont->Release();
                continue;
            }

            res = pUnknown->QueryInterface(IID_ID3DXEffect, &pObject);

            if (SUCCEEDED(res) && pObject != NULL)
            {
                ID3DXEffect *pEffect = (ID3DXEffect *)pObject;
                pEffect->OnLostDevice();
                pEffect->Release();
                continue;
            }

            res = pUnknown->QueryInterface(IID_ID3DXMesh, &pObject);

            if (SUCCEEDED(res) && pObject != NULL)
            {
                ID3DXMesh *pMesh = (ID3DXMesh *)pObject;
                pMesh->Release();
                continue;
            }

        }
    }
}

void CEngine::OnResetDevice()
{
    Debug(L"Device reset!\n");

    for (unsigned int i = 0; i < m_vecObjectList.size(); i++)
    {
        HRESULT res = S_OK;
        IUnknown *pUnknown = m_vecObjectList[i];

        if ( pUnknown != NULL)
        {
            void *pObject = NULL;

            res = pUnknown->QueryInterface(IID_ID3DXSprite, &pObject);

            if (res == S_OK && pObject != NULL)
            {
                ID3DXSprite *pSprite = (ID3DXSprite *)pObject;
                pSprite->OnResetDevice();
                pSprite->Release();
                continue;
            }

            res = pUnknown->QueryInterface(IID_ID3DXLine, &pObject);

            if (SUCCEEDED(res) && pObject != NULL)
            {
                ID3DXLine *pLine = (ID3DXLine *)pObject;
                pLine->OnResetDevice();
                pLine->Release();
                continue;
            }

            res = pUnknown->QueryInterface(IID_ID3DXFont, &pObject);

            if (SUCCEEDED(res) && pObject != NULL)
            {
                ID3DXFont *pFont = (ID3DXFont *)pObject;
                pFont->OnResetDevice();
                pFont->Release();
                continue;
            }

            res = pUnknown->QueryInterface(IID_ID3DXEffect, &pObject);

            if (SUCCEEDED(res) && pObject != NULL)
            {
                ID3DXEffect *pEffect = (ID3DXEffect *)pObject;
                pEffect->OnResetDevice();
                pEffect->Release();
                continue;
            }
        }

    }

    SetActive(true);
}

void CEngine::Destroy()
{
    Zap(m_pSplashSprite);
    Zap(m_pLogoSprite);

    if (m_pGame)
    {
        m_pGame->Destroy();
    }

    if (g_pGameFactory)
    {
        g_pGameFactory->Destroy();
    }

    // This takes care of all CComPtr's by itself, they don't need to be 
    // Release()'d any further since the container takes care of this
    m_vecObjectList.clear();

    // Destroy game systems
    for (unsigned int i = 0; i < m_vecGameSystems.size(); i++)
    {
        if (m_vecGameSystems[i] != NULL)
        {
            m_vecGameSystems[i]->Destroy();

            //Zap(m_vecGameSystems[i]);
        }
    }

    m_vecGameSystems.clear();

    g_pTextureLoader->Reset();

    if (m_pd3ddev != NULL)
    {
        m_pd3ddev->Release();
    }

    if (m_pd3d != NULL)
    {
        ZapDX(m_pd3d);
    }
}

void CEngine::Error( String error )
{
    MessageBox(m_hwnd, error.c_str(), TEXT("Error"), MB_OK | MB_ICONERROR );
}

void CEngine::FatalError( String error )
{
    Error(error);
    PostQuitMessage(0);
    m_bFailed = true;
}

void CEngine::Message( String msg )
{
    MessageBox(m_hwnd, msg.c_str(), TEXT("Message"), MB_OK | MB_ICONINFORMATION );
}

void CEngine::RenderSplashScreens()
{
    if ( Input()->KeyPressed(VK_SPACE) )
    {
        m_iSplashCount = 2;
        m_pStartupSound->Stop();
        return;
    }

    D3DXVECTOR3 center( 0.5f*SPLASH_IMAGE_X, 0.5f*SPLASH_IMAGE_Y, 0.0f );
    D3DXVECTOR3 position( 0.5f*m_iScreenX, 0.5f*m_iScreenY, 0.0f );

    Camera()->Update();

    if (m_iSplashCount == 0)
    {
        if (!m_tFadeTimer.IsStarted())
        {
            m_pStartupSound = Sound()->CreateSample(L"startup.wav");
            m_pStartupSound->Play();

            m_tFadeTimer.Start();
        }

        if (m_pLogoSprite != NULL)
        {
            m_pLogoSprite->SetRenderColorUniform( (int)(255 * m_tFadeTimer.GetModifier()) );
            m_pLogoSprite->Render();
        }

        if (m_tFadeTimer.IsFinished())
        {
            m_tFadeTimer.Reset();
            m_iSplashCount = 1;
        }
    }

    if (m_iSplashCount == 1)
    {
        if (!m_tFadeTimer.IsStarted())
        {
            m_tFadeTimer.Start();
        }


        if (m_pSplashSprite != NULL)
        {
            m_pSplashSprite->SetRenderColorUniform( (int)(255 * m_tFadeTimer.GetModifier()) );
            m_pSplashSprite->Render();
        }

        if (m_tFadeTimer.IsFinished())
        {
            m_tFadeTimer.Reset();
            m_iSplashCount = 2;
        }
    }
}