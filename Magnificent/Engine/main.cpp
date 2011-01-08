#include "base.h"

//#define NOGDI
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

#undef ERROR

#include "engine.h"
#include "Shlwapi.h"
#include "StrSafe.h"

#include <iostream>
#include <fstream>

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    // Set up the proper parent directory
    // If we're in a bin folder, move one folder up
    wchar_t currentDir[MAX_PATH];
    wchar_t *pCurrentDir = currentDir;

    GetCurrentDirectory( MAX_PATH, pCurrentDir );
    String current_dir = pCurrentDir;

    if ( current_dir.find(L"\\bin\0") != wstring::npos ) // Test for terminator at the end as well
    {
        wchar_t *pNewCurrentDir = new wchar_t[MAX_PATH+1];
        //strncpy_s(pNewCurrentDir, sizeof(pNewCurrentDir), pCurrentDir, sizeof(pCurrentDir) );
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

    CEngine *pEngine = new CEngine();

    if ( pEngine->Init(hInstance) )
    {
        pEngine->Start();
    }

    pEngine->Stop();

    delete pEngine;

    _CrtDumpMemoryLeaks();
    return 0;
}