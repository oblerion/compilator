#pragma once
#include "raylib.h"
#include <stdlib.h>
#include <string.h>

#if defined(__linux)
#define LIBMAN_CONFIGPATH "/home/$USER/.compilator"
#elif defined(WIN32)
#define LIBMAN_CONFIGPATH "C:\compilator"
#endif
struct libmanager
{
    int b;
};

struct libmanager libmanager_init();