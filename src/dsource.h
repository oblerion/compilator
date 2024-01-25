#pragma once
#include "raylib.h"
#include <stdio.h>
#define DSDIR_MAX 30
enum SourceType
{
    ST_C=0,
    ST_CPP
};

struct dsource_file
{
    char name[30];
    enum SourceType type;
};

struct dsource_file GetSource(const char* cfile); 
void PrintSource(struct dsource_file dfile);
bool SourceIsEgal(struct dsource_file dfile,struct dsource_file dfile2);

struct dsource_dir
{
    int count;
    struct dsource_file files[DSDIR_MAX];
    char name[30];
};

struct dsource_dir ScanSourceDir(const char* cdir);

#define SOURCES_MAX 30

struct sources 
{
    int count;
    struct dsource_file files[SOURCES_MAX];
};

struct sources sources();
void sources_add(struct sources* s,struct dsource_file dfile);
void sources_update(struct sources* s,struct dsource_dir dfile);