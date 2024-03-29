#pragma once
#include "raylib.h"
#include <stdio.h>

#define DFILE_MAX 30
#define DDIR_MAX 30
enum IncludeType
{
    IT_SYSTEM=0,
    IT_LOCAL
};

struct dinclude
{
    char name[30];
    enum IncludeType type;
};

struct dinclude_file
{
    int count;
    struct dinclude list[DFILE_MAX];
    char name[30];
    char path[50];
};

struct dinclude_file GetInclude(const char* cfile);
void PrintInclude(struct dinclude_file dfile);
bool IncludeIsEgal(struct dinclude di1,struct dinclude di2);

struct dinclude_dir
{
    int count;
    struct dinclude_file files[DDIR_MAX];
    char name[30];
};

struct dinclude_dir ScanIncludeDir(const char* cdir);

#define DEPENDS_MAX 50

struct depends
{
    int count;
    struct dinclude list[DEPENDS_MAX];
};
struct depends depends();
int depends_find(struct depends* dp,const char* name);
void depends_add(struct depends* dp,struct dinclude dinc);
void depend_update(struct depends* dp, struct dinclude_dir sidir);