#include "base.h"

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

#undef ERROR

#include "engine.h"
#include "Shlwapi.h"
#include "StrSafe.h"

#include <iostream>
#include <fstream>

#ifdef SCREENSAVER
#include <ScrnSave.h>

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "ComCtl32.lib")


UINT_PTR uTimer = NULL; 

BOOL WINAPI ScreenSaverConfigureDialog(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam)
{
    return true;
}
BOOL WINAPI RegisterDialogClasses(HANDLE hmodule)
{
    return true;
}

CEngine *pEngine = NULL;

LONG WINAPI ScreenSaverProc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam)
{
    // Handles screen saver messages
    switch(message)
    {
    case WM_CREATE:
        // Creation of the screen saver window

        pEngine = CEngine::GetInstance();

        if ( pEngine->Init( 0 ) )
        {
            
            uTimer = SetTimer(hwnd, 1, 10, NULL);
            pEngine->Start();
        }
        else
        {
            pEngine->Stop();
        }

        return 0;
    case WM_ERASEBKGND:
        // Erases the screen saver background
        return 0;
    case WM_TIMER:
        // Handles the timer
        CEngine::GetInstance()->FrameAdvance();

        return 0;
    case WM_DESTROY:
        // Cleans up the screen saver window
        KillTimer(hwnd, uTimer);
        CEngine::GetInstance()->Stop();

        PostQuitMessage(0);
        return 0;
    }
    return DefScreenSaverProc(hwnd,message,wparam,lparam);
}

#else

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
#if 0
    // Set up the proper parent directory
    // If we're in a bin folder, move one folder up
    wchar_t currentDir[MAX_PATH];
    wchar_t *pCurrentDir = currentDir;

    GetCurrentDirectory( MAX_PATH, pCurrentDir );
    String current_dir = pCurrentDir;

    if ( current_dir.find(L"\\bin\0") != wstring::npos ) // Test for terminator at the end as well
    {
        wchar_t *pNewCurrentDir = new wchar_t[MAX_PATH+1];
        StringCchPrintfW(pNewCurrentDir, MAX_PATH, L"%s", pCurrentDir);

        // Get the parent path.
        PathRemoveFileSpec( pNewCurrentDir );

        if ( !SetCurrentDirectory( pNewCurrentDir )  )
        {
            OutputDebugString( L"SetCurrentDirectory failed with error " );
            OutputDebugString( (LPCWSTR)GetLastError() );
            OutputDebugString( L"\n" );

            return 0;
        }
        
        current_dir = pNewCurrentDir;
        delete pNewCurrentDir;
    }

/*
    std::ofstream outFile("Magnificent.log", std::ios::out | std::ios::binary);
    outFile.write((char *)(L"Current directory is: " + current_dir + L"\r\n").c_str(), 24*sizeof(wchar_t) + current_dir.length() * sizeof(wchar_t));
    
    std::wofstream woutFile("Magnificent2.log", std::ios::out);
    String line = L"Current directory is: " + current_dir;
    woutFile.write( line.c_str(), line.length() );
*/

    current_dir.clear();
#endif

    CEngine *pEngine = CEngine::GetInstance();

    if ( pEngine->Init(hInstance) )
    {
        pEngine->Start();
    }

    pEngine->Stop();

    //_CrtDumpMemoryLeaks();
    return 0;
}

#endif