/*******************************************************************************************
 *
 *   raylib [core] example - Basic window (adapted for HTML5 platform)
 *
 *   This example is prepared to compile for PLATFORM_WEB, PLATFORM_DESKTOP and PLATFORM_RPI
 *   As you will notice, code structure is slightly diferent to the other examples...
 *   To compile it for PLATFORM_WEB just uncomment #define PLATFORM_WEB at beginning
 *
 *   This example has been created using raylib 1.3 (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
 *
 *   Copyright (c) 2015 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "../include/raylib.h"
#include "../include/raygui.h"
#include <stdio.h>
#include <stdlib.h>

//#define PLATFORM_WEB

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

 
//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
int screenWidth = 800;
int screenHeight = 600;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void); // Update and Draw one frame

// Spot data
typedef struct {
    Vector2 pos;
    Vector2 vel;
    float inner;
    float radius;

    // Shader locations
    unsigned int posLoc;
    unsigned int innerLoc;
    unsigned int radiusLoc;
} Spot;




//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main()
{
  // Initialization
  //--------------------------------------------------------------------------------------
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  //SetAudioStreamBufferSizeDefault(MAX_SAN);
  
  InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
  InitAudioDevice();

  

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------
  
  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    UpdateDrawFrame();
  }
#endif


  CloseAudioDevice();
  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void)
{
  // Update
  //----------------------------------------------------------------------------------
  // TODO: Update your variables here
  //----------------------------------------------------------------------------------

  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();

  ClearBackground(GRAY);


  //DrawTexture(building, 0, 0, WHITE);
  //DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
  //DrawRectangle(100, 100, 100, 300, RED);
  //DrawRectangleV((Vector2){300, 200}, (Vector2){100, 30}, BLUE);
  //sysDraw.update(&e);
  
  EndDrawing();
  //----------------------------------------------------------------------------------
}

