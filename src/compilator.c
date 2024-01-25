#include "compilator.h"

struct compilator compilator()
{
    struct compilator cr;
    cr.depends = depends();
    cr.sources = sources();
    cr.objs = (struct objs){0};
    cr.berror = false;
    FilePathList files = LoadDirectoryFiles(".");
    for(int i=0;i<files.count;i++)
    {
        if(DirectoryExists(files.paths[i]))
        {
            depend_update(&cr.depends,ScanIncludeDir(files.paths[i]));
            sources_update(&cr.sources,ScanSourceDir(files.paths[i]));
        }
    }
    if(cr.sources.count>0)
        cr.projecttype = cr.sources.files[0].type;
    UnloadDirectoryFiles(files);
    printf("--------- compilator ver %s ---------- BEGIN\n",COMPILATOR_VERSION);
    return cr;
}

bool compilator_findlib(struct compilator compilator, const char *name)
{
    if(depends_find(&compilator.depends,name) > -1) 
        return true;
    return false;
}

bool compilator_isinstalledlib(const char *name)
{
#if defined(__linux)
    const char* slib = TextFormat("/usr/local/lib/lib%s.a",GetFileNameWithoutExt(name));
    const char* sinc = TextFormat("/usr/local/include/%s",name);
#else 
    const char* slib="";
    const char* sinc="";
#endif
    if(FileExists(slib) &&
       FileExists(sinc))
    {
        return true;
    }
    return false;
}

void compilator_comp(struct compilator* compilator)
{
    puts("compile : BEGIN");
    const char* sARG = "-g -Wall";
    char sCC[10];
    const char* sDOBJ = "build/linux64/obj/";

    #if defined(__linux)
        if(compilator->projecttype==ST_C)
        {
            strcpy(sCC,"clang");
        }
        else if(compilator->projecttype==ST_CPP)
        {
            strcpy(sCC,"clang++");
        }
    #endif

    for(int i=0;i<compilator->sources.count;i++)
    {
        if(compilator->projecttype==ST_C)
        {
            const char* sfile = compilator->sources.files[i].name;
            const char* sfileo = TextFormat("%s.o",GetFileNameWithoutExt(sfile));
            const char* dfileo = TextFormat("%s%s",sDOBJ,sfileo);
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
                break;
            }
        }
    }
    puts("compile : END");
}

void compilator_link(struct compilator compilator)
{
    puts("link : BEGIN");
    char sARG[100] = "-g -Wall";
    char sCC[10];
    char sOBJ[1500]="";

    #if defined(__linux)
        if(compilator.projecttype==ST_C)
        {
            strcpy(sCC,"clang");
        }
        else if(compilator.projecttype==ST_CPP)
        {
            strcpy(sCC,"clang++");
        }
    #endif

    if(compilator.berror==false)
    {
        if(compilator_findlib(compilator,"raylib.h") && compilator_isinstalledlib("raylib.h"))
        {
    #if defined(__linux)
            strcat(sARG," -lraylib -lGL -lm -lpthread -ldl -lrt -lX11");
    #endif
        }

        if(compilator_findlib(compilator,"lua.h") && compilator_isinstalledlib("lua.h"))
        {
    #if defined(__linux)
            strcat(sARG," -llua");
    #endif
        }

        for(int i=0;i<compilator.objs.count;i++)
        {
            if(i>0) strcat(sOBJ," ");
            strcat(sOBJ,compilator.objs.path[i]);
        }
        const char* ssys = TextFormat("%s %s %s -o main",sCC,sOBJ,sARG);
        puts(ssys);
        system(ssys);
    }
    puts("link : END");
    printf("--------- compilator ver %s ---------- END\n",COMPILATOR_VERSION);
}
