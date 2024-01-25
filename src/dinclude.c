#include "dinclude.h"

const char* str_inc = "#include ";

struct dinclude_file GetInclude(const char *cfile)
{
    struct dinclude_file dfile={0};
    const char* ext = GetFileExtension(cfile);
    if(TextIsEqual(ext,".h") || TextIsEqual(ext,".hpp"))
    {
        char* sdata = LoadFileText(cfile);
        TextCopy(dfile.name,cfile);
        for(int i=0;i<TextLength(sdata);i++)
        {
            for(int j=0;j<TextLength(str_inc);j++)
            {
                if(str_inc[j]!=sdata[i+j])
                {
                    break;
                }
                else 
                {
                    if(j==TextLength(str_inc)-1 && dfile.count<30)
                    {
                        struct dinclude dinclude;
                        //puts(TextFormat("#include in %d %c",i,sdata[i+j+1]));
                        char cbg = sdata[i+j+1];
                        char cend = '0';
                        if(cbg == '<'){ cend = '>'; dinclude.type=IT_SYSTEM;}
                        else{ cend = '"'; dinclude.type=IT_LOCAL;}
                        int pos = i+j+2;
                        while(sdata[pos++]!=cend);
                        TextCopy(dinclude.name ,TextSubtext(sdata,i+j+2,(pos-1)-(i+j+2)));
                        dfile.list[dfile.count++]=dinclude;
                        //puts(TextSubtext(sdata,i+j+2,(pos-1)-(i+j+2)));
                    }
                }
            }
        }
        UnloadFileText(sdata);
    }
    return dfile;
}

void PrintInclude(struct dinclude_file dfile)
{
    for(int i=0;i<dfile.count;i++)
    {
        puts(TextFormat("i %d",i));
        puts(dfile.list[i].name);
        puts(TextFormat("type %d",dfile.list[i].type));
    }
}

bool IncludeIsEgal(struct dinclude di1, struct dinclude di2)
{
    return TextIsEqual(di1.name,di2.name) && di1.type==di2.type;
}

struct dinclude_dir ScanIncludeDir(const char *cdir)
{
    struct dinclude_dir ddir={0};
    TextCopy(ddir.name,cdir);
    FilePathList files = LoadDirectoryFiles(cdir);
    for(int i=0;i<files.count;i++)
    {
        if(IsPathFile(files.paths[i]) && ddir.count<DDIR_MAX)
        {
            struct dinclude_file dfile = GetInclude(files.paths[i]);
            TextCopy(dfile.path, TextFormat("%s/%s",cdir,dfile.name));
            ddir.files[ddir.count++] = dfile;
        }
    }
    UnloadDirectoryFiles(files);
    return ddir;
}

struct depends depends()
{
    struct depends dp={0};
    return dp;
}

int depends_find(struct depends *dp, const char *name)
{
    for(int i=0;i<dp->count;i++)
    {
        if(TextIsEqual(dp->list[i].name,name)) return i;
    }
    return -1;
}

void depends_add(struct depends *dp, struct dinclude dinc)
{
    for(int i=0;i<dp->count;i++)
    {
        if(IncludeIsEgal(dp->list[i],dinc)) return;
    }
    if(dp->count<DEPENDS_MAX)
    {
        dp->list[dp->count++] = dinc;
    }
}

void depend_update(struct depends *dp, struct dinclude_dir sidir)
{
    for(int i=0;i<sidir.count;i++)
    for(int j=0;j<sidir.files[i].count;j++)
    {
        depends_add(dp,sidir.files[i].list[j]);
    }

}
