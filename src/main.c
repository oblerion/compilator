#include "raylib.h"
#include "dinclude.h"
int main(void)
{

    // Initialization
    //--------------------------------------------------------------------------------------
   // float fraq = GetMonitorWidth(0)/GetMonitorHeight(0);
    const int screenWidth = 1000; //GetMonitorWidth(0);
    const int screenHeight = 600;//GetMonitorHeight(0);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");


    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //Font f = LoadFont(".ttf");
    // struct dfile dfile = GetInclude("src/depend.h");
    // PrintInclude(dfile);
    struct dinclude_dir ddir =  ScanIncludeDir("src");
    for(int i=0;i<ddir.count;i++)
    {
        puts(ddir.files[i].name);
        PrintInclude(ddir.files[i]);
    }
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        //if(IsKeyDown(KEY_S))
       
        BeginDrawing();
        ClearBackground(BLUE);

        EndDrawing();
    }
   // UnloadModel(model);
    CloseWindow();        // Close window and OpenGL
    return 0;
}

