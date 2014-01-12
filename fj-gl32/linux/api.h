#ifndef FJ_GL32_API_LINUX_HEAD
#define FJ_GL32_API_LINUX_HEAD
#define FJ_GL32_API_HEAD
#include <string>

#ifdef _WIN32
#error Including a Linux-specific header on Windows.
#endif

#ifndef __linux__
#warning Including a Linux-specific header on a different (non-Windows) platform.
#endif

#define EXPORT(x) extern "C" x

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

namespace FJ{
    namespace GLX{
        extern Display *display;
        extern Window window;
        extern GLXContext glc;
    }
}

EXPORT(bool InitVideo(int w, int h, std::string sphere_dir));

#include "../api.h"
#endif
