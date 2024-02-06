#include "compilator.h"
int main(int narg, char** larg)
{
    struct compilator cr={0};
#if defined(__linux)
    if(narg==1) 
        cr = compilator("linux64");
    else 
    {
        for(int i=0;i<narg;i++)
        {
            if(TextIsEqual(larg[i],"--target") &&
            i+1<narg)
            {
                if(TextIsEqual(larg[i+1],"linux64") ||
                TextIsEqual(larg[i+1],"win32") ||
                TextIsEqual(larg[i+1],"win64") ||
                TextIsEqual(larg[i+1],"web"))
                {
                    cr = compilator(larg[i+1]);
                }
            }
        }
    }
#elif defined(WIN32)
    if(narg==1) cr = compilator("win32");
#elif defined(WIN64)
    if(narg==1) cr = compilator("win64");
#endif
    compilator_comp(&cr);
    compilator_link(&cr);
    if(cr.berror)
    {
        InitWindow(1200,600,TextFormat("compilator ver %s",COMPILATOR_VERSION));
        SetTargetFPS(60);
        while(!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground(WHITE);
            DrawText(TextFormat("%s",cr.error_file),30,50,40,BLACK);
            EndDrawing();

        }
        CloseWindow();
    }
    return 0;
}

