#ifndef FJ_GL32_API_WIN32_HEAD
#define FJ_GL32_API_WIN32_HEAD
#define FJ_GL32_API_HEAD

#ifndef _WIN32
#error Including a Windows-specific header on a non-Windows platform.
#endif


#include <Windows.h>
#include <Wingdi.h>

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#include <GL/gl.h>

#define EXPORT(x) extern "C" __declspec(dllexport) x

EXPORT(bool  InitVideoDriver(HWND window, int screen_width, int screen_height));
EXPORT(void  ConfigureDriver(HWND parent));



#include "../api.h"
#endif
