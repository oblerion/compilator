#include "libmanager.h"
#define URL_RAYLIB "https://github.com/raysan5/raylib/releases/download/5.0"
#define ZIP_RAYLIB_WIN32 "raylib-5.0_win32_mingw-w64.zip"
#define ZIP_RAYLIB_WIN64 "raylib-5.0_win64_mingw-w64.zip"
#define ZIP_RAYLIB_WEB "raylib-5.0_webassembly.zip"

const char* _GetFileNameWithoutExt(const char *filePath)
{
    if (filePath != NULL)
    {
        const char *fileName = GetFileName(filePath); // Get filename.ext without path
        for (int i = (int)strlen(fileName); i>0; i--) // Reverse search '.'
        {
            if (fileName[i] == '.')
            {
                char rfileName[i];
                strncpy(rfileName, fileName,i);
                rfileName[i] = '\0';
                const char* rrfileName = rfileName;
                return rrfileName;
            }
        }
        return fileName; // Extension not found
    }
    return "\0";
}

void install_lib(const char* ptarget,const char* purl,const char* pzip)
{
    #define dtarget TextFormat("%s/%s",LIBMAN_CONFIGPATH,ptarget)
    #define dzip _GetFileNameWithoutExt(pzip)

    system(TextFormat("wget -P %s %s/%s",dtarget,purl,pzip));
    system(TextFormat("unzip %s/%s -d %s",dtarget,pzip,dtarget));
    system(TextFormat("mv %s/%s/lib %s",dtarget,dzip,dtarget));
    system(TextFormat("mv %s/%s/include %s",dtarget,dzip,dtarget));
    system(TextFormat("rm -r %s/%s",dtarget,dzip));
    system(TextFormat("rm -r %s/%s",dtarget,pzip));
}

void install_libs(int id)
{
#if defined(__linux)
    switch(id)
    {
        case 0:
            install_lib("win32",URL_RAYLIB,ZIP_RAYLIB_WIN32);
        break;
        case 1:
            install_lib("win64",URL_RAYLIB,ZIP_RAYLIB_WIN64);
        break;
        case 2:
            install_lib("web",URL_RAYLIB,ZIP_RAYLIB_WEB);
        break;
        default:;
    }
#endif
}

struct libmanager libmanager_init()
{
    struct libmanager lm={0};
#if defined(__linux)
    const char* win32_path = TextFormat("%s/win32",LIBMAN_CONFIGPATH);
    const char* win64_path = TextFormat("%s/win64",LIBMAN_CONFIGPATH);
    const char* web_path = TextFormat("%s/web",LIBMAN_CONFIGPATH);
    
    system(TextFormat("if [ ! -d %s ]; then mkdir %s fi;",LIBMAN_CONFIGPATH,LIBMAN_CONFIGPATH));
    system(TextFormat("if [ ! -d %s ]; then mkdir %s fi;",win32_path,win32_path));
    system(TextFormat("if [ ! -d %s ]; then mkdir %s fi;",win64_path,win64_path));
    system(TextFormat("if [ ! -d %s ]; then mkdir %s fi;",web_path,web_path));
    install_libs(0);
#elif defined(WIN32)
    const char* win32_path = TextFormat("%s/win32",LIBMAN_CONFIGPATH);

    system(TextFormat("IF NOT EXIST %s (mkdir %s)",win32_path,win32_path));
#endif
    return lm;
}

