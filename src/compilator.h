#pragma once
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dinclude.h"
#include "dsource.h"

#define COMPILATOR_VERSION "a0.3"

const char* GetUserLinux();

#if defined(__linux)
#define COMPILATOR_CONFIGPATH "/opt/compilator"
#elif defined(WIN32)
#define COMPILATOR_CONFIGPATH "C:\compilator"
#endif

struct objs
{
    int count;
    char path[30][100];
};

struct compilator
{
    struct depends depends;
    struct sources sources;
    struct objs objs;
    bool berror;
    enum SourceType projecttype;
    char target[10];
    char error_file[50];
};

struct compilator compilator(const char* target);
// find lib into depends
bool compilator_findlib(struct compilator compilator,const char* name);
// find lib into config path
bool compilator_isinstalledlib(const char* name,const char* target);

void compilator_comp(struct compilator* compilator);
void compilator_link(struct compilator* compilator);