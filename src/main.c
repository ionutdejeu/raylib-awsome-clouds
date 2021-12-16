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
#define ECS_IMPLEMENTATION
#include "ecs.h"


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
//Define our components 
typedef struct
{
    float x, y;
} CTransform;

typedef enum
{
    COMPONENT_TRANSFORM,
    COMPONENT_VELOCITY,
    COMPONENT_SPRITE,
    COMPONENT_MISSLE,

    COMPONENT_COUNT
} ComponentType;


#define MOVEMENT_SYSTEM_MASK \
ECS_MASK(1, COMPONENT_TRANSFORM)
void 
movement_system(Ecs *ecs)
{
    for (int i = 0; i < ecs_for_count(ecs); i++)
    {
        EcsEnt e = ecs_get_ent(ecs, i);
        if (ecs_ent_has_mask(ecs, e, MOVEMENT_SYSTEM_MASK))
        {
            CTransform *xform   = ecs_ent_get_component(ecs, e, COMPONENT_TRANSFORM);
        }
    }
}

void 
register_components(Ecs *ecs)
{
    //Ecs, component index, component pool size, size of component, and component free func
    ecs_register_component(ecs, COMPONENT_TRANSFORM, 1000, sizeof(CTransform), NULL);
  
}

void 
register_systems(Ecs *ecs)
{
    //ecs_run_systems will run the systems in the order they are registered
    //ecs_run_system is also available if you wish to handle each system seperately
    //
    //Ecs, function pointer to system (must take a parameter of Ecs), system type
    ecs_register_system(ecs, movement_system, ECS_SYSTEM_UPDATE);
}
Ecs *ecs;
EcsEnt e;
Shader shaderClouds;
double startTime = 0;

unsigned int iTimeLocation; 
//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main()
{
  // Initialization
  //--------------------------------------------------------------------------------------
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  //SetAudioStreamBufferSizeDefault(MAX_SAN);
  
  InitWindow(screenWidth, screenHeight, "Clouds simulation shader");
  InitAudioDevice();
  shaderClouds = LoadShader(0, "../../resources/clouds.fs");
  iTimeLocation = GetShaderLocation(shaderClouds, "iTime");

  //Max entities, component count, system_count
  ecs = ecs_make(1000, COMPONENT_COUNT, 3);
  register_components(ecs);
  register_systems(ecs);

  e = ecs_ent_make(ecs);
  CTransform xform = {0, 0};
  ecs_ent_add_component(ecs, e, COMPONENT_TRANSFORM, &xform);    
  startTime = GetTime();

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
  

  ecs_ent_destroy(ecs, e);

  ecs_destroy(ecs);

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

  ecs_run_systems(ecs, ECS_SYSTEM_UPDATE);
  float diff = startTime - GetTime(); 
  SetShaderValue(shaderClouds, iTimeLocation,&diff, SHADER_UNIFORM_FLOAT);

  // Update
  //----------------------------------------------------------------------------------
  // TODO: Update your variables here
  //----------------------------------------------------------------------------------

  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();

  ClearBackground(GRAY);
  
  BeginShaderMode(shaderClouds);
      // Instead of a blank rectangle you could render here
      // a render texture of the full screen used to do screen
      // scaling (slight adjustment to shader would be required
      // to actually pay attention to the colour!)
      DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
  EndShaderMode();


  //DrawTexture(building, 0, 0, WHITE);
  DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
  //DrawRectangle(100, 100, 100, 300, RED);
  //DrawRectangleV((Vector2){300, 200}, (Vector2){100, 30}, BLUE);
  //sysDraw.update(&e);
  
  EndDrawing();
  //----------------------------------------------------------------------------------
}

