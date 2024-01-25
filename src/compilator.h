#pragma once
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include "dinclude.h"
#include "dsource.h"

#define COMPILATOR_VERSION "a0.1"

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
};

struct compilator compilator();
bool compilator_findlib(struct compilator compilator,const char* name);
bool compilator_isinstalledlib(const char* name);
void compilator_comp(struct compilator* compilator);
void compilator_link(struct compilator compilator);