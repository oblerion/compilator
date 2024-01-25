#include "dsource.h"

struct dsource_file GetSource(const char *cfile)
{
    struct dsource_file dsource;
    TextCopy(dsource.name,cfile);
    const char* ext = GetFileExtension(cfile);
    if(TextIsEqual(ext,".c"))
    {
        dsource.type = ST_C;
    }
    else if(TextIsEqual(ext,".cpp"))
    {
        dsource.type = ST_CPP;
    }
    else dsource.type = -1;
    return dsource;
}

void PrintSource(struct dsource_file dfile)
{
    puts(dfile.name);
    puts(TextFormat("source type=%d",dfile.type));
}

bool SourceIsEgal(struct dsource_file dfile,struct dsource_file dfile2)
{
    return TextIsEqual(dfile.name,dfile2.name) && dfile.type==dfile2.type;
}

struct dsource_dir ScanSourceDir(const char *cdir)
{
    struct dsource_dir dsdir = {0};
    TextCopy(dsdir.name,cdir);
 
    FilePathList files = LoadDirectoryFiles(cdir);
    for(int i=0;i<files.count;i++)
    {
        if(IsPathFile(files.paths[i]) && dsdir.count<DSDIR_MAX)
        {
            struct dsource_file dfile = GetSource(files.paths[i]);
            if(dfile.type!=-1)
                dsdir.files[dsdir.count++] = dfile;
        }
    }
    UnloadDirectoryFiles(files);
    return dsdir;
}

struct sources sources()
{
    struct sources s = {0};
    return s;
}

void sources_add(struct sources *s,struct dsource_file dfile)
{
    if(s->count<SOURCES_MAX)
    {
        s->files[s->count++] = dfile;
    }
}

void sources_update(struct sources *s,struct dsource_dir dfile)
{
    for(int i=0;i<dfile.count;i++)
    {
        sources_add(s,dfile.files[i]);
    }
}
