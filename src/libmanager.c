#include "libmanager.h"

struct libmanager libmanager_init()
{
    struct libmanager lm;
    const char* win32_path = TextFormat("%s/win32",LIBMAN_CONFIGPATH);
    const char* web_path = TextFormat("%s/web",LIBMAN_CONFIGPATH);
    
    system(TextFormat("if [ ! -d %s ]; then mkdir %s ;fi",LIBMAN_CONFIGPATH,LIBMAN_CONFIGPATH));
    system(TextFormat("if [ ! -d %s ]; then mkdir %s ;fi",win32_path,win32_path));
    system(TextFormat("if [ ! -d %s ]; then mkdir %s ;fi",web_path,web_path));

    return lm;
}
