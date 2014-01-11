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

EXPORT(bool InitVideo(int w, int h, std::string sphere_dir));

#include "../api.h"
#endif
