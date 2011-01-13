//////////////////////////////////////////////////////////////////////////
//
//  Magnificent Game Engine
//
//  engine.h
//
//  Header file for singleton engine object. This class is the main hub 
//  of the game application; it takes care of D3D and all objects 
//  related to it, the windows, the message pump, and all the sub systems 
//  in the game, such as sound, input and physics
//
//

#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include <memory>
#include "game_systems.h"
#include "timer.h"
#include "smart_ptr.h"
#include "gameinterface.h"
#include "sound.h"

#include "sprite.h"

using namespace std;

class CEngine 
{
public:
    CEngine();
    //~CEngine();

    // Startup/Shutdown
    bool Init( HINSTANCE hInstance );
    void Destroy();

    void Start();
    void Main();
    void Stop();

    // Focus handlers
    void OnLostDevice();
    void OnResetDevice();

    // Reset the device, changing resolution etc
    bool ResetDirect3D(bool fullscreen);

    // Frame handling 
    void FrameAdvance();

    void RenderSplashScreens();
    void PreRender();
    void PostRender();

    // Garbage collection
    void AddObject( IUnknown* object ) { m_vecObjectList.push_back(object); }

    // Game system handling
    void AddGameSystem( IGameSystem* subSystem ) { m_vecGameSystems.push_back(subSystem); }

    void ParseCommandLine() {}

    // Error handling
    void Error( String error  );
    void FatalError( String error );
    void Message( String msg );

    inline void SetActive( bool newState ) { m_bActive = newState; }
    inline bool IsActive() { return m_bActive; }
    inline bool IsPaused() { return !m_bActive; }
    
#ifdef DEBUG
    void Debug( String msg ) { OutputDebugString( (msg).c_str() ); }
#else
    void Debug( String msg ) {}
#endif

    void ChangeWindow();

    //void RegisterGameFactory(IGameFactory* factory) { g_pGameFactory = factory; }
    static LRESULT CALLBACK MessagePump(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    inline bool HasFailed() { return m_bFailed; }
    inline void SetFailed() { m_bFailed = true; }

    inline IDirect3DDevice9* GetDevice() { return m_pd3ddev; }
    float ScreenWidth()             { return (float)m_iScreenX; }
    float ScreenHeight()            { return (float)m_iScreenY; }

    void DisableSplashScreen() { m_bSplashScreenDisabled = true; }

protected:

    // Various init functions
    bool InitWindow(int x, int y, bool fullscreen);
    void CreateSystems();
    bool InitSystems();
    bool InitDirect3D(bool fullscreen);

private:

    // Game interface
    IGameFactory*                   m_pGameFactory;
    IGameInterface*                 m_pGame;
    bool                            m_bGameStarted;

    // Screen properties
    bool                            m_bFullscreen;
    int                             m_iScreenX;
    int                             m_iScreenY;

    // Engine is running and not paused
    bool                            m_bActive;

    // DirectX 9 Objects
    LPDIRECT3D9                     m_pd3d; // It's not safe to use the pointer container on this
    CComPtr<IDirect3DDevice9>       m_pd3ddev;

    // Splash screen resources
    CSprite                         *m_pLogoSprite;
    CSprite                         *m_pSplashSprite;
    CSample                         *m_pStartupSound;

    // Handling splash screens
    bool ShouldRenderSplashes()     
    { 
        if (m_bSplashScreenDisabled)
            return false;
        else if (m_iSplashCount < 2)
            return false;
        return true;
    }

    bool                            m_bSplashScreenDisabled;
    int                             m_iSplashCount;
    CFadeTimer                      m_tFadeTimer;

    CTimer                          m_tProfileTimer;

    // Window(s) properties
    HINSTANCE                       m_hInstance;
    HWND                            m_hwnd;
    RECT ClientRect;                // Saved client rectangle
    RECT WindowRect;                // Windowed windows rectangle

    // Manages all DX objects
    vector<IUnknown*>               m_vecObjectList;

    // All subsystems in the game engine is listed here
    vector<IGameSystem*>            m_vecGameSystems;
    
    bool m_bFailed; // True if the engine has encountered a fatal error
};

extern CEngine* g_sInstance;

extern inline CEngine* Engine()
{
    return g_sInstance;
}

#endif // ENGINE_H