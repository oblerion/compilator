#include "compilator.h"

bool _IfTestRequire(struct compilator* comp)
{
    bool rbool = true;
    if(!DirectoryExists("asset") && TextIsEqual(comp->target,"web"))
    {
        comp->berror = true;
        strcpy(comp->error_file,"need asset dir for web build");
        rbool = false;
    }
    else if(!DirectoryExists("src"))
    {
        comp->berror = true;
        strcpy(comp->error_file,TextFormat("src dir not exist"));
        rbool = false;
    }
    return rbool;
}
#if defined(__linux)
void _InitDir_Linux(struct compilator comp)
{
    char DOUT[20]="build/";
    strcat(DOUT,comp.target);
    if(TextIsEqual(comp.target,"web"))
    {
        system(TextFormat("if [ ! -d %s/obj ]; then mkdir -p %s/obj; fi",DOUT,DOUT));
        system(TextFormat("if [ ! -f %s/index.html ]; then cp %s/web/index.html %s/.; fi",DOUT,COMPILATOR_CONFIGPATH, DOUT));
    }
    else 
        system(TextFormat("if [ ! -d %s ]; then mkdir -p %s; fi",DOUT,DOUT));
}
#endif
#if defined(WIN32)
void _InitDir_Window(struct compilator comp)
{

    const char* DOUT = TextFormat("build/%s",comp.target);
    if(TextIsEqual(comp.target,"web"))
    {
        system(TextFormat("if not exist %c%s%c (mkdir %s/obj)",'"',DOUT,'"',DOUT));
        system(TextFormat("if not exist %c%s/index.html%c (cp %s/web/index.html %s/.)",'"',DOUT,'"',COMPILATOR_CONFIGPATH,DOUT));
    }
    else 
        system(TextFormat("if not exist %c%s%c (mkdir %s)",'"',DOUT,'"',DOUT));

}
#endif
struct compilator compilator(const char* target)
{
    struct compilator cr;
    cr.depends = depends();
    cr.sources = sources();
    cr.objs = (struct objs){0};
    cr.berror = false;
    strcpy(cr.target,target);

    // init out dir 
#if defined(__linux)
    _InitDir_Linux(cr);
#elif defined(WIN32)
    _InitDir_Window(cr);
#endif

    if(_IfTestRequire(&cr))
    {
        depend_update(&cr.depends,ScanIncludeDir("src"));
        sources_update(&cr.sources,ScanSourceDir("src"));
        if(cr.sources.count>0)
            cr.projecttype = cr.sources.files[0].type;
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

const char* _GetCC(struct compilator* compilator)
{
    const char* sCC = NULL;
    #if defined(__linux)
        if(compilator->projecttype==ST_C)
        {
            if(TextIsEqual(compilator->target,"linux64"))
                sCC = TextFormat("%s","clang");
            else if(TextIsEqual(compilator->target,"win32"))
                sCC = TextFormat("%s","i686-w64-mingw32-gcc");
            else if(TextIsEqual(compilator->target,"win64"))
                sCC = TextFormat("%s","x86_64-w64-mingw32-gcc");
            else if(TextIsEqual(compilator->target,"web"))
                sCC = TextFormat("%s","emcc");
        }
        else if(compilator->projecttype==ST_CPP)
        {
            if(TextIsEqual(compilator->target,"linux64"))
                sCC = TextFormat("%s","clang++");
            else if(TextIsEqual(compilator->target,"win32"))
                sCC = TextFormat("%s","i686-w64-mingw32-g++");
            else if(TextIsEqual(compilator->target,"win64"))
                sCC = TextFormat("%s","x86_64-w64-mingw32-g++");
            else if(TextIsEqual(compilator->target,"web"))
                sCC = TextFormat("%s","em++");
        }
    #elif defined(WIN32) || defined(WIN64)
        if(compilator->projecttype==ST_C)
        {
            sCC = TextFormat("%s","gcc");
        }
        else if(compilator->projecttype==ST_CPP)
        {
            sCC = TextFormat("%s","g++");
        }
    #endif
    return sCC;
}

const char* _GetDOBJ(struct compilator compilator)
{
    const char* sDOBJ = NULL;
    if(TextIsEqual(compilator.target,"web"))
        sDOBJ = TextFormat("build/%s/obj/",compilator.target);
    else 
        sDOBJ = TextFormat("build/%s/",compilator.target);       
    return sDOBJ;
}

const char* _GetARG_comp(struct compilator compilator)
{
    char sARG[300] = "";
    const char* sARG1 = TextFormat("-g -Wall -I%s/%s/include ",COMPILATOR_CONFIGPATH,compilator.target);
    const char* sARG2_web = "-Isrc -I. ";

#if defined(__linux)
    const char* sARG2_win32 = "-I/usr/i696-w64-mingw32/include ";
    const char* sARG2_win64 = "-I/usr/x86_64-w64-mingw32/include ";
#endif

    if(TextIsEqual(compilator.target,"linux64"))
    {
        strcat(sARG,sARG1);
    }
    else if(TextIsEqual(compilator.target,"web"))
    {
        strcat(sARG,sARG1);
        strcat(sARG,sARG2_web);
    }
    else if(TextIsEqual(compilator.target,"win32"))
    {
#if defined(__linux)
        strcat(sARG,sARG1);
        strcat(sARG,sARG2_win32);
#else 
        strcat(sARG,sARG1);
#endif
    }
    else if(TextIsEqual(compilator.target,"win64"))
    {
#if defined(__linux)
        strcat(sARG,sARG1);
        strcat(sARG,sARG2_win64);
#else
        strcat(sARG,sARG1);
#endif
    }
    return TextFormat("%s",sARG);;
}

bool _Compile(struct compilator* compilator,const char* sCC,const char* sfile,const char* sARG,const char* sDOBJ)
{
    bool rbool = true;
    const char* dfileo = TextFormat("%s%s.o",sDOBJ,GetFileNameWithoutExt(sfile));
    const char* ssys = TextFormat("%s -c %s %s -o %s",sCC, sfile, sARG, dfileo);
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
        rbool = false;
    }
    return rbool;
}

void compilator_comp(struct compilator* compilator)
{
    puts("compile : BEGIN");
    if(!compilator->berror)
    {
        char sARG[200];
        strcpy(sARG,_GetARG_comp(*compilator));
        char sCC[20];
        strcpy(sCC,_GetCC(compilator));
        char sDOBJ[60];
        strcpy(sDOBJ,_GetDOBJ(*compilator));
   
        for(int i=0;i<compilator->sources.count;i++)
        {
            if(!_Compile(compilator,sCC,compilator->sources.files[i].name,sARG,sDOBJ))
                break;
        }
    }
    puts("compile : END");
}

const char* _GetARG_link(struct compilator* compilator)
{
    char sARG[200] = "";
    const char* sARG_noWeb = TextFormat("-L%s/%s/lib -g -Wall",COMPILATOR_CONFIGPATH,compilator->target); 
    
    const char* sARG_raylib_linux = " -lraylib -lGL -lm -lpthread -ldl -lrt -lX11";
    const char* sARG_raylib_win32 = " -lraylib -lopengl32 -lgdi32 -lwinmm";
    const char* sARG_raylib_web = TextFormat(" %s/%s/lib/libraylib.a",COMPILATOR_CONFIGPATH,compilator->target);
    
    const char* sARG_lua_web = TextFormat(" %s/%s/lib/liblua.a",COMPILATOR_CONFIGPATH,compilator->target);
    const char* sARG_lua = " -llua";

    const char* sARG_Win_C = " -mwindows -static-libgcc";
    const char* sARG_Win_Cpp = " -mwindows -static-libgcc -static-libstdc++";

    if(!TextIsEqual(compilator->target,"web"))
        strcat(sARG,sARG_noWeb);

    // raylib add lib
    if(compilator_findlib(*compilator,"raylib.h") && 
    compilator_isinstalledlib("raylib.h",compilator->target))
    {                
        if(TextIsEqual(compilator->target,"linux64"))
        {
            strcat(sARG,sARG_raylib_linux);
        }
        else if(TextIsEqual(compilator->target,"win32"))
        {
            strcat(sARG,sARG_raylib_win32);
        }
        else if(TextIsEqual(compilator->target,"web"))
        {
            strcat(sARG,sARG_raylib_web);
        }
    }

    // lua add lib
    if(compilator_findlib(*compilator,"lua.h") && 
    compilator_isinstalledlib("lua.h",compilator->target))
    {
        if(TextIsEqual(compilator->target,"web"))
            strcat(sARG,sARG_lua_web);
        else 
            strcat(sARG,sARG_lua);
    }

    // std add lib
    if(TextIsEqual(compilator->target,"win32") || 
        TextIsEqual(compilator->target,"win64"))
    {
#if defined(__linux) || defined(WIN32) || defined(WIN64)
        if(compilator->projecttype==ST_C) 
            strcat(sARG,sARG_Win_C);
        else if(compilator->projecttype==ST_CPP) 
            strcat(sARG,sARG_Win_Cpp);
#endif
    }
    return TextFormat("%s",sARG);
}

const char* _GetSOBJ(struct compilator* compilator)
{
    const int size = compilator->objs.count*100;
    char sOBJ[size];
    // get all obj
    for(int i=0;i<compilator->objs.count;i++)
    {
        if(i>0) strcat(sOBJ," ");
        strcat(sOBJ,compilator->objs.path[i]);
    }
    return TextFormat("%s",sOBJ);
}

void _Link(struct compilator* compilator,const char* sCC,const char* sOBJ,const char* sARG)
{
    const char* execname = GetFileName(GetWorkingDirectory());
    const char* ssys = NULL;
    // output file
    if(TextIsEqual(compilator->target,"linux64"))
    {
        ssys = TextFormat("%s %s %s -o %s",sCC,sOBJ,sARG,execname);
    }
    else if(TextIsEqual(compilator->target,"win32") || 
        TextIsEqual(compilator->target,"win64"))
    {
        ssys = TextFormat("%s %s %s -o %s.exe",sCC,sOBJ,sARG,execname);
    }
    else if(TextIsEqual(compilator->target,"web"))
    {
        ssys = TextFormat("%s -o build/web/index.js build/web/obj/*.o -Os -Wall %s -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB --preload-file ./asset",sCC,sARG);   
    }
    puts(ssys);
    system(ssys);
    if(!TextIsEqual(compilator->target,"web") && 
        !FileExists(execname) && 
        !FileExists(TextFormat("%s.exe",execname)) ) 
    {
        compilator->berror=true;
        strcpy(compilator->error_file,"Link Error");
    }
}

void compilator_link(struct compilator* compilator)
{
    puts("link : BEGIN");
    const char* sARG = _GetARG_link(compilator);
    const char* sCC = _GetCC(compilator);
    const char* sOBJ = NULL;

    if(!compilator->berror)
    {
        sOBJ = _GetSOBJ(compilator);
       _Link(compilator,sCC,sOBJ,sARG);
    }
    puts("link : END");
    printf("--------- compilator ver %s ---------- END\n",COMPILATOR_VERSION);
}
