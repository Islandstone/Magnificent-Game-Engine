#include "base.h"
#include "engine.h"
#include "dx_error_check.h"
#include "timer.h"
#include "input.h"
#include "camera.h"
#include "texture.h"

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

CEngine* g_sInstance = NULL;
bool     g_bActive = true;
bool     g_bPastInitDelay = false;
bool     g_bChangingWindow = false;

CEngine::CEngine()
{
    m_bFullscreen = false;
    m_bFailed = false;
    m_bGameStarted = false;

    m_iScreenX = 0;
    m_iScreenY = 0;

    m_iSplashCount = 0;

    m_pd3d = NULL;

    m_bActive = true;

    m_hInstance = NULL;
    m_hwnd = NULL;  

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

    //static TCHAR szAppName[] = TEXT ("Magnificent");

    WNDCLASSEX   wndclassex = {0};

    wndclassex.cbSize        = sizeof(WNDCLASSEX);
    wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc   = &MessagePump;
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

    D3DPRESENT_PARAMETERS window_params;
    ZeroMemory( &window_params, sizeof(D3DPRESENT_PARAMETERS) );

    window_params.BackBufferWidth         = m_iScreenX;
    window_params.BackBufferHeight        = m_iScreenY;
    window_params.BackBufferFormat        = D3DFMT_UNKNOWN;
    window_params.BackBufferCount         = 1;

    window_params.MultiSampleType         = D3DMULTISAMPLE_NONE; //D3DMULTISAMPLE_8_SAMPLES;
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
    full_params.BackBufferFormat        = D3DFMT_A8R8G8B8;
    full_params.BackBufferCount         = 1;

    full_params.MultiSampleType         = D3DMULTISAMPLE_NONE; //D3DMULTISAMPLE_8_SAMPLES;
    full_params.MultiSampleQuality      = 0;

    full_params.SwapEffect              = D3DSWAPEFFECT_DISCARD;
    full_params.hDeviceWindow           = m_hwnd;
    full_params.Windowed                = FALSE;
    full_params.EnableAutoDepthStencil  = FALSE;
    full_params.AutoDepthStencilFormat  = D3DFMT_D32;
    full_params.Flags = 0;

    full_params.FullScreen_RefreshRateInHz = 60; /* FullScreen_RefreshRateInHz must be zero for Windowed mode */
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
        Error(L"Failed to create D3D device!");
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

    /*
    if (    FAILED( D3DXCreateSprite( m_pd3ddev, &m_pLogoSprite ) )
        ||  FAILED( D3DXCreateTextureFromFile( m_pd3ddev, L"logo.bmp", &m_pLogoTexture) )
        ||  FAILED( D3DXCreateTextureFromFile( m_pd3ddev, L"splash.bmp", &m_pSplashTexture) ) 
        ||  FAILED( D3DXCreateSprite( m_pd3ddev, &m_pSplashSprite) )
        )       
    {
        Debug(L"Failed to create sprites/load textures for logo or splash");
    }
    else
    {   
        AddObject(m_pLogoSprite);
        AddObject(m_pLogoTexture);
        AddObject(m_pSplashSprite);
        AddObject(m_pSplashTexture);
    }
    */

    m_pLogoSprite = new CSprite(L"logo.bmp");
    m_pSplashSprite = new CSprite(L"splash.bmp");

    /*
    g_pCamera->SetPosition(0.0f,0.0f);

    m_pd3ddev->SetTransform( D3DTS_PROJECTION,  &g_pCamera->GetProjection() );
    m_pd3ddev->SetTransform( D3DTS_VIEW,        &g_pCamera->GetView() );
    */

    return true;
}

bool CEngine::ResetDirect3D(bool fullscreen)
{
    if (m_pd3d == NULL)
    {
        Error(L"Failed to create D3D interface!");
        return false;
    }

    D3DPRESENT_PARAMETERS window_params;
    ZeroMemory( &window_params, sizeof(D3DPRESENT_PARAMETERS) );

    window_params.BackBufferWidth         = m_iScreenX;
    window_params.BackBufferHeight        = m_iScreenY;
    window_params.BackBufferFormat        = D3DFMT_UNKNOWN;
    window_params.BackBufferCount         = 1;

    window_params.MultiSampleType         = D3DMULTISAMPLE_NONE; //D3DMULTISAMPLE_8_SAMPLES;
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
    full_params.BackBufferFormat        = D3DFMT_A8R8G8B8;
    full_params.BackBufferCount         = 1;

    full_params.MultiSampleType         = D3DMULTISAMPLE_NONE; //D3DMULTISAMPLE_8_SAMPLES;
    full_params.MultiSampleQuality      = 0;

    full_params.SwapEffect              = D3DSWAPEFFECT_DISCARD;
    full_params.hDeviceWindow           = m_hwnd;
    full_params.Windowed                = FALSE;
    full_params.EnableAutoDepthStencil  = FALSE;
    full_params.AutoDepthStencilFormat  = D3DFMT_D32;
    full_params.Flags = 0;

    full_params.FullScreen_RefreshRateInHz = 60; /* FullScreen_RefreshRateInHz must be zero for Windowed mode */
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
    AddGameSystem( new CSoundSystem() );
    AddGameSystem( new CInput() );
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

    if (g_pGlobalTimer)
    {
        g_pGlobalTimer->Start();
    }

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

            FrameAdvance();

            // Try not to kill the computer we're running on
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
    // Update game systems
    for (unsigned int i = 0; i < m_vecGameSystems.size(); i++)
    {
        if (m_vecGameSystems[i] != NULL)
        {
            m_vecGameSystems[i]->Update();
        }
#ifdef DEBUG
        else
        {
            // NULL pointer to game system in vector should never happen
            DebugBreak();
        }
#endif
    }

    if ( ShouldRenderSplashes() )
    {
        // Using a delay timer so the engine doesn't pop when startin
        if (g_bPastInitDelay)
        {
            PreRender();
            if (IsActive())
            {
                RenderSplashScreens();
            }
            PostRender();
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
    else if (!m_bGameStarted)
    {
        m_bGameStarted = true;
        m_pGame->Start();
    }

    if (m_pGame == NULL)
    {
        return;
    }

    m_pGame->PreThink();
    m_pGame->Think();
    m_pGame->PostThink();

    if ( g_pInput->KeyReleased( VK_F2 ) )
    {
        ChangeWindow();
    }

    PreRender();
    m_pGame->PreRender();

    m_pGame->Render();

    // Render game systems
    for (unsigned int i = 0; i < m_vecGameSystems.size(); i++)
    {
        if (m_vecGameSystems[i] != NULL)
        {
            m_vecGameSystems[i]->Render();
        }
    }

    m_pGame->PostRender();
    PostRender();

    // PostRender game systems
    for (unsigned int i = 0; i < m_vecGameSystems.size(); i++)
    {
        if (m_vecGameSystems[i] != NULL)
        {
            m_vecGameSystems[i]->PostRender();
        }
    }
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

LRESULT CALLBACK CEngine::MessagePump(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//#define PAINTTEST

#ifdef PAINTTEST
    HDC hdc;
    PAINTSTRUCT ps;

    String text(L"Intermission, brb in 5 minutes");
#endif

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
        
    case WM_KEYDOWN:
        if ((lParam & 1<<30) == 0)
        {
            g_pInput->SetKeyDown(wParam);
        }
        return (0);
    case WM_KEYUP:
        g_pInput->SetKeyUp(wParam);

        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }

        return (0);

#ifdef PAINTTEST
    case WM_PAINT:
        hdc = BeginPaint (hwnd, &ps);
        TextOut(hdc, 10, 10, text.c_str(), text.size() );
        EndPaint (hwnd, &ps);
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
        return DefWindowProc (hwnd, message, wParam, lParam);
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

            Zap(m_vecGameSystems[i]);
        }
#ifdef DEBUG
        else
        {
            OutputDebugString(L"Null pointer in game systems vector\n");
        }
#endif
    }

    m_vecGameSystems.clear();

    g_pTextureLoader->Reset();

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
    if ( g_pInput->KeyPressed(VK_SPACE) )
    {
        m_iSplashCount = 2;
        m_pStartupSound->Stop();
        return;
    }

    D3DXVECTOR3 center( 0.5f*SPLASH_IMAGE_X, 0.5f*SPLASH_IMAGE_Y, 0.0f );
    D3DXVECTOR3 position( 0.5f*m_iScreenX, 0.5f*m_iScreenY, 0.0f );

    g_pCamera->Update();

    if (m_iSplashCount == 0)
    {
        if (!m_tFadeTimer.IsStarted())
        {
            /*
            D3DXMATRIX projectionMatrix;
            D3DXMATRIX viewMatrix;

            D3DXMatrixOrthoOffCenterLH(&projectionMatrix,
                (float)m_iScreenX*0.5f, (float)m_iScreenX*0.5f,
                (float)m_iScreenY*0.5f, -(float)m_iScreenY*0.5f,
                0.0f, 10.0f);

            //D3DXMatrixOrthoLH(&projectionMatrix, (float)m_iScreenX*0.5f, (float)m_iScreenY*0.5f, 0.0f, 10.0f);
            m_pd3ddev->SetTransform( D3DTS_PROJECTION, &projectionMatrix);

            // Set the camera's view matrix
            D3DXVECTOR3 eye(0.0f, 0.0f, -1.0f);
            D3DXVECTOR3 at(0.0f, 0.0f, 0.0f);

            // Assumes that the up vector is along the y-axis, ie. x is to the right and z is downwards/inwards
            D3DXMatrixLookAtLH(&viewMatrix, &eye, &at, &D3DXVECTOR3(0.0f,1.0f,0.0f));
            m_pd3ddev->SetTransform( D3DTS_VIEW, &viewMatrix);
            */

            m_pStartupSound = g_pSound->CreateSample(L"startup.wav");
            m_pStartupSound->Play();

            m_tFadeTimer.Start();
        }

        if (m_pLogoSprite != NULL /*&& m_pLogoTexture != NULL*/)
        {
            /*
            m_pLogoSprite->Begin( 0 );
            D3DCOLOR clr = D3DCOLOR_ARGB( 255, (int)(255.0f*m_tFadeTimer.GetModifier()), (int)(255.0f*m_tFadeTimer.GetModifier()), (int)(255.0f*m_tFadeTimer.GetModifier()) );
            m_pLogoSprite->Draw(m_pLogoTexture, NULL, &center, &position, clr );
            m_pLogoSprite->End();
            */
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


        if (m_pSplashSprite != NULL /*&& m_pSplashTexture != NULL*/)
        {
            /*
            m_pSplashSprite->Begin( 0 );
                        D3DCOLOR clr = D3DCOLOR_ARGB( 255, (int)(255.0f*m_tFadeTimer.GetModifier()), (int)(255.0f*m_tFadeTimer.GetModifier()), (int)(255.0f*m_tFadeTimer.GetModifier()) );
                        m_pSplashSprite->Draw(m_pSplashTexture, NULL, &center, &position, clr );
                        m_pSplashSprite->End();*/
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