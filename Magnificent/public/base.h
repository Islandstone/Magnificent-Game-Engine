#ifndef BASE_H
#define BASE_H

#ifdef _DEBUG
#define DEBUG
#endif

#ifdef DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <string>

#define APPNAME TEXT("Magnificent")

#define FULLSCREEN FALSE

#define SCREEN_X_WIN 1280
#define SCREEN_Y_WIN 768

#define SCREEN_X_FULL 1920
#define SCREEN_Y_FULL 1200

#if FULLSCREEN
#define SCREEN_SIZE_X SCREEN_X_FULL
#define SCREEN_SIZE_Y SCREEN_Y_FULL
#else
#define SCREEN_SIZE_X SCREEN_X_WIN
#define SCREEN_SIZE_Y SCREEN_Y_WIN
#endif

#define SPLASH_IMAGE_X 512.0f
#define SPLASH_IMAGE_Y 512.0f

#define INIT_DELAY 1.5f

#define Zap(x) if (x != NULL) {delete x; x = NULL;}
#define ZapDX(x) if (x != NULL) { x->Release(); x = NULL;}

// Global typedefs
typedef std::wstring String;

#endif // BASE_H