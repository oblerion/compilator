#include "depend.h"

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
            ddir.files[ddir.count++]= GetInclude(files.paths[i]);
    }
    UnloadDirectoryFiles(files);
    return ddir;
}
