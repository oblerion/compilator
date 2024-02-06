#include "compilator.h"
const char* GetUserLinux()
{
    char* rstr=NULL;
#if defined(__linux)
    #include <pwd.h>
    #include <unistd.h>
    struct passwd* pw = getpwuid(geteuid());
    if(pw)
        rstr = pw->pw_name;
#endif
    return rstr;
}

struct compilator compilator(const char* target)
{
    struct compilator cr;
    cr.depends = depends();
    cr.sources = sources();
    cr.objs = (struct objs){0};
    cr.berror = false;
    strcpy(cr.target,target);

    // init out dir 
    const char* DOUT = TextFormat("build/%s",target);
    #if defined(__linux)
        if(TextIsEqual(target,"web"))
        {
            system(TextFormat("if [ ! -d %s/obj ]; then mkdir -p %s/obj; fi",DOUT,DOUT));
            system(TextFormat("if [ ! -f %s/index.html ]; then cp %s/web/index.html %s/.; fi",DOUT,COMPILATOR_CONFIGPATH, DOUT));
        }
        else 
            system(TextFormat("if [ ! -d %s ]; then mkdir -p %s; fi",DOUT,DOUT));
    #endif

    if(!DirectoryExists("asset") && TextIsEqual(target,"web"))
    {
        cr.berror = true;
        strcpy(cr.error_file,"need asset dir for web build");
    }
    else if(DirectoryExists("src"))
    {
        depend_update(&cr.depends,ScanIncludeDir("src"));
        sources_update(&cr.sources,ScanSourceDir("src"));
        if(cr.sources.count>0)
            cr.projecttype = cr.sources.files[0].type;
    }
    else 
    {
        cr.berror = true;
        strcpy(cr.error_file,TextFormat("src dir not exist"));
    }

    printf("--------- compilator ver %s ---------- BEGIN\n",COMPILATOR_VERSION);
    return cr;
}

bool compilator_findlib(struct compilator compilator, const char *name)
{
    if(depends_find(&compilator.depends,name) > -1) 
    {
        return true;
    }
    return false;
}

bool compilator_isinstalledlib(const char *name, const char *target)
{
    char slib[60]="";
    strcpy(slib,TextFormat("%s/%s/lib/lib%s.a",COMPILATOR_CONFIGPATH,target,GetFileNameWithoutExt(name)));
    char sinc[60]="";
    strcpy(sinc,TextFormat("%s/%s/include/%s",COMPILATOR_CONFIGPATH,target,name));

    if(FileExists(slib) &&
       FileExists(sinc))
    {
        return true;
    }
    return false;
}

const char* _GetCompiler(struct compilator* compilator)
{
    #if defined(__linux)
        if(compilator->projecttype==ST_C)
        {
            if(TextIsEqual(compilator->target,"linux64"))
                return TextFormat("%s","clang");
            else if(TextIsEqual(compilator->target,"win32"))
                return TextFormat("%s","i686-w64-mingw32-gcc");
            else if(TextIsEqual(compilator->target,"win64"))
                return TextFormat("%s","x86_64-w64-mingw32-gcc");
            else if(TextIsEqual(compilator->target,"web"))
                return TextFormat("%s","emcc");
        }
        else if(compilator->projecttype==ST_CPP)
        {
            if(TextIsEqual(compilator->target,"linux64"))
                return TextFormat("%s","clang++");
            else if(TextIsEqual(compilator->target,"win32"))
                return TextFormat("%s","i686-w64-mingw32-g++");
            else if(TextIsEqual(compilator->target,"win64"))
                return TextFormat("%s","x86_64-w64-mingw32-g++");
            else if(TextIsEqual(compilator->target,"web"))
                return TextFormat("%s","em++");
        }
    #elif defined(WIN32) || defined(WIN64)
        if(compilator->projecttype==ST_C)
        {
            return TextFormat("%s","gcc");
        }
        else if(compilator->projecttype==ST_CPP)
        {
            return TextFormat("%s","g++");
        }
    #endif
    return "";
}

void compilator_comp(struct compilator* compilator)
{
    if(!compilator->berror)
    {
        puts("compile : BEGIN");
        char sARG[100]="";
        strcpy(sARG,TextFormat("-g -Wall -I%s/%s/include ",COMPILATOR_CONFIGPATH,compilator->target));
        char sCC[20]="";
        strcpy(sCC,_GetCompiler(compilator));
        char sDOBJ[60]="";
        if(TextIsEqual(compilator->target,"web"))
            strcpy(sDOBJ,TextFormat("build/%s/obj/",compilator->target));
        else 
            strcpy(sDOBJ,TextFormat("build/%s/",compilator->target));
        
        if(TextIsEqual(compilator->target,"web"))
        {
            strcat(sARG,"-Isrc -I.");
        }
        else if(!DirectoryExists(sDOBJ)) 
            system(TextFormat("mkdir %s",sDOBJ));

    #if defined(__linux)
        if(TextIsEqual(compilator->target,"win32"))
        {
            strcat(sARG,"-I/usr/i696-w64-mingw32/include ");
        }
        else if(TextIsEqual(compilator->target,"win64"))
        {
            strcat(sARG,"-I/usr/x86_64-w64-mingw32/include ");
        }
    #endif
        char sfile[30]="";
        char sfileo[30]="";
        char dfileo[60]="";
        char ssys[200]="";
        for(int i=0;i<compilator->sources.count;i++)
        {
            strcpy(sfile, compilator->sources.files[i].name);
            strcpy(sfileo,TextFormat("%s.o",GetFileNameWithoutExt(sfile)));
            strcpy(dfileo,TextFormat("%s%s",sDOBJ,sfileo));

            strcpy(ssys,TextFormat("%s -c %s %s -o %s",sCC, sfile, sARG, dfileo));
            puts(ssys);
            system(ssys);
            if(FileExists(dfileo))
            {
                TextCopy(compilator->objs.path[compilator->objs.count++], dfileo);
            }
            else 
            {
                compilator->berror=true;
                puts("COMPILE ERROR");
                strcpy(compilator->error_file,TextFormat("Compile Error file: %s",sfile));
                break;
            }
        }
        puts("compile : END");
    }
}

void compilator_link(struct compilator* compilator)
{
    puts("link : BEGIN");
    char sARG[300]="";
    char sCC[20]="";
    strcpy(sCC,_GetCompiler(compilator));
    char sOBJ[1500]="";

    if(!compilator->berror)
    {
        if(!TextIsEqual(compilator->target,"web"))
            strcpy(sARG,TextFormat("-L%s/%s/lib -g -Wall",COMPILATOR_CONFIGPATH,compilator->target));

        // raylib add lib
        if(compilator_findlib(*compilator,"raylib.h") && 
        compilator_isinstalledlib("raylib.h",compilator->target))
        {                
            if(TextIsEqual(compilator->target,"linux64"))
            {
                strcat(sARG," -lraylib -lGL -lm -lpthread -ldl -lrt -lX11");
            }
            else if(TextIsEqual(compilator->target,"win64"))
            {
                strcat(sARG," -lraylib -lraylibdll -lopengl32 -lgdi32 -lwinmm");
            }
            else if(TextIsEqual(compilator->target,"win32"))
            {
                strcat(sARG," -lraylib -lopengl32 -lgdi32 -lwinmm");
            }
            else if(TextIsEqual(compilator->target,"web"))
            {
                strcat(sARG,TextFormat(" %s/%s/lib/libraylib.a",COMPILATOR_CONFIGPATH,compilator->target));
            }
        }
        // lua add lib
        if(compilator_findlib(*compilator,"lua.h") && 
        compilator_isinstalledlib("lua.h",compilator->target))
        {
            if(TextIsEqual(compilator->target,"web"))
            {
                strcat(sARG,TextFormat(" %s/%s/lib/liblua.a",COMPILATOR_CONFIGPATH,compilator->target));
            }
            else 
                strcat(sARG," -llua");
        }

        // std add lib
        if(TextIsEqual(compilator->target,"win32") || 
            TextIsEqual(compilator->target,"win64"))
        {
    #if defined(__linux) || defined(WIN32) || defined(WIN64)
            if(compilator->projecttype==ST_C) 
                strcat(sARG, " -mwindows -static-libgcc");
            else if(compilator->projecttype==ST_CPP) 
                strcat(sARG, " -mwindows -static-libgcc -static-libstdc++");
    #endif
        }

        // get all obj
        for(int i=0;i<compilator->objs.count;i++)
        {
            if(i>0) strcat(sOBJ," ");
            strcat(sOBJ,compilator->objs.path[i]);
        }
        char ssys[500]="";
        #define execname GetFileName(GetWorkingDirectory())
        // output file
        if(TextIsEqual(compilator->target,"linux64"))
        {
            strcpy(ssys,TextFormat("%s %s %s -o %s",sCC,sOBJ,sARG,execname));
        }
        else if(TextIsEqual(compilator->target,"win32") || 
            TextIsEqual(compilator->target,"win64"))
        {
            strcpy(ssys,TextFormat("%s %s %s -o %s.exe",sCC,sOBJ,sARG,execname));
        }
        else if(TextIsEqual(compilator->target,"web"))
        {
            //strcpy(ssys,TextFormat("%s %s %s -o %s.exe",sCC,sOBJ,sARG,execname));
            strcpy(ssys,TextFormat("%s -o build/web/index.js build/web/obj/*.o -Os -Wall %s -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB --preload-file ./asset",sCC,sARG));
//emcc -o build/web/index.js src/*.o -Os -Wall $RAYLIBWEB/lib/libraylib.a -I. -Iinclude -L. -Llib -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB --preload-file ./asset;
        }
        puts(ssys);
        system(ssys);
        if(!FileExists(execname) && !FileExists(TextFormat("%s.exe",execname)) ) 
        {
            compilator->berror=true;
            strcpy(compilator->error_file,"Link Error");
        }
        puts("link : END");
    }
  
    printf("--------- compilator ver %s ---------- END\n",COMPILATOR_VERSION);
}
