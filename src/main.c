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
#include "../raylib/src/raylib.h"
#include "../include/raygui.h"
#include "../raylib/src/raymath.h"
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

// Spot data

typedef struct {
    Vector2 vel;
    float inner;
    float radius;
    bool isActive;

    // Shader locations
    unsigned int posLoc;
    unsigned int innerLoc;
    unsigned int radiusLoc;
    unsigned int isActiveLoc;

} Spot;

typedef struct {
  Color *tintsArray;
} Sun;

typedef struct { 
  int percent;
  Color c;
} ColorKey;

typedef struct {
  Texture2D *sprite;
  ColorKey *gradientKeys;
  int colorKeyCount; 
} SriteComponent;


Spot s;
Ecs *ecs;
EcsEnt e;
EcsEnt *clouds;
Shader shaderClouds;
Shader shaderSun;
Texture2D mountain;
Texture2D sun; 

typedef enum
{
    COMPONENT_TRANSFORM,
    COMPONENT_VELOCITY,
    COMPONENT_SPRITE,
    COMPONENT_MISSLE,
    COMPONENT_CLOUD_SPOT,
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

float Func(float pX)
{
	return sin(pX);
}

#define CLOUD_SYSTEM_MASK \
ECS_MASK(2, COMPONENT_TRANSFORM,COMPONENT_CLOUD_SPOT)
void 
cloud_update_system(Ecs *ecs)
{
    for (int i = 0; i < ecs_for_count(ecs); i++)
    {
        EcsEnt e = ecs_get_ent(ecs, i);
        if (ecs_ent_has_mask(ecs, e, CLOUD_SYSTEM_MASK))
        {
            CTransform *xform   = ecs_ent_get_component(ecs, e, COMPONENT_TRANSFORM);
            Spot *s   = ecs_ent_get_component(ecs, e, COMPONENT_CLOUD_SPOT);
            s->isActive = true;
            // update position based on speed 
            Vector2 transform = {xform->x,xform->y};
            Vector2 new_pos = Vector2Add(transform,Vector2Scale((s->vel),GetFrameTime()));

            xform->x = new_pos.x;
            xform->y = new_pos.y;
            int act = 1;

            // if mouse is pressed
            if(IsMouseButtonPressed(0)){
                // check overlapp 
                bool overlapp = CheckCollisionCircles(new_pos,s->inner,(Vector2){GetMouseX(),GetMouseY()},s->radius);
                if(overlapp){
                    printf("clicked on cloud");
                }
            }

            DrawCircle(xform->x, xform->y+Func(xform->x)*10, 50, (Color){234,247,252,255});                               
            DrawCircle(20+xform->x+Func(xform->y)*50*GetFrameTime(),33+xform->y+cos(xform->y)*50*GetFrameTime(), 50, (Color){192,232,245,255});                               
            DrawCircle(-20+xform->x+Func(xform->y)*20*GetFrameTime(),43+xform->y+cos(xform->y)*10*GetFrameTime(), 50, (Color){234,247,252,255});                               
            DrawCircle(-2+xform->x+Func(xform->y)*60*GetFrameTime(),-23+xform->y+cos(xform->y)*0.2*GetFrameTime(), 50, (Color){192,232,245,255});                               
            DrawCircle(-80+xform->x+Func(xform->y)*60*GetFrameTime(),60+xform->y+cos(xform->y)*4*GetFrameTime(), 50, (Color){192,232,245,255});                               

            SetShaderValue(shaderClouds, s->posLoc, &new_pos, SHADER_UNIFORM_VEC2);
            SetShaderValue(shaderClouds, s->isActiveLoc, &act, SHADER_UNIFORM_INT);

        }
    }
}

void 
register_components(Ecs *ecs)
{
    //Ecs, component index, component pool size, size of component, and component free func
    ecs_register_component(ecs, COMPONENT_TRANSFORM, 1000, sizeof(CTransform), NULL);
    ecs_register_component(ecs, COMPONENT_CLOUD_SPOT, 1000, sizeof(Spot), NULL);
}

void 
register_systems(Ecs *ecs)
{
    //ecs_run_systems will run the systems in the order they are registered
    //ecs_run_system is also available if you wish to handle each system seperately
    //
    //Ecs, function pointer to system (must take a parameter of Ecs), system type
    ecs_register_system(ecs, movement_system, ECS_SYSTEM_UPDATE);
    ecs_register_system(ecs, cloud_update_system, ECS_SYSTEM_CLOUD_UPDATE);
}


double startTime = 0;

Vector2 getRandomCloudSpawnPos(){
  // needs to be somewhere in the last row of the clouds 
  return (Vector2){40,40};
}

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
  shaderSun = LoadShader(0, "../../resources/circle.fs");
  shaderClouds = LoadShader(0, "../../resources/clouds.fs");
  mountain = LoadTexture("../../resources/mountain.png");
  sun = LoadTexture("../../resources/sun.png");

  iTimeLocation = GetShaderLocation(shaderClouds, "iTime");


  //Max entities, component count, system_count
  ecs = ecs_make(1000, COMPONENT_COUNT, 3);
  register_components(ecs);
  register_systems(ecs);

  e = ecs_ent_make(ecs);
  CTransform xform = {0, 0};
  ecs_ent_add_component(ecs, e, COMPONENT_TRANSFORM, &xform);    
  startTime = GetTime();

  Spot *cloudsStruct =  malloc(sizeof(Spot)*3);
  for(int i = 0;i<3;i++){
    EcsEnt *cloud = ecs_ent_make(ecs);

    CTransform cloudTransform = {0, 0};
    char posName[32] = "spots[x].pos\0";
    char innerName[32] = "spots[x].inner\0";
    char radiusName[32] = "spots[x].radius\0";
    char isActive[32] = "spots[x].isActive\0";

    posName[6] = '0' + i;
    innerName[6] = '0' + i;
    radiusName[6] = '0' + i;
    isActive[6] = '0' + i;


    cloudsStruct[i].posLoc = GetShaderLocation(shaderClouds, posName);
    cloudsStruct[i].innerLoc = GetShaderLocation(shaderClouds, innerName);
    cloudsStruct[i].radiusLoc = GetShaderLocation(shaderClouds, radiusName);
    cloudsStruct[i].isActiveLoc = GetShaderLocation(shaderClouds, isActive);

    Vector2 randPos = (Vector2){100,100};
    cloudTransform.x = (float)randPos.x;
    cloudTransform.y = (float)randPos.y;
    cloudsStruct[i].vel = (Vector2){10, 10};

    cloudsStruct[i].inner = 28.0f * (i + 1);
    cloudsStruct[i].radius = 48.0f * (i + 1);
    cloudsStruct[i].isActive = true;
    
    ecs_ent_add_component(ecs, cloud, COMPONENT_TRANSFORM, &cloudTransform);   
    ecs_ent_add_component(ecs, cloud, COMPONENT_CLOUD_SPOT, &cloudsStruct[i]);

    int act = 1;
    SetShaderValue(shaderClouds, cloudsStruct[i].posLoc, &cloudTransform, SHADER_UNIFORM_VEC2);
    SetShaderValue(shaderClouds, cloudsStruct[i].innerLoc, &cloudsStruct[i].inner, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shaderClouds, cloudsStruct[i].radiusLoc, &cloudsStruct[i].radius, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shaderClouds, cloudsStruct[i].isActiveLoc, &act, SHADER_UNIFORM_INT);
  }



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
  Vector2 origin = (Vector2){0,0};
  
  DrawRectangle(0,0,screenWidth,screenWidth,(Color){67, 166, 198,255});
  DrawTexturePro(sun,
    (Rectangle){0,0,sun.width,sun.height},
    (Rectangle){10,100,100,100},
    origin,
    0,WHITE);  //DrawTexture(building, 0, 0, WHITE);
  BeginShaderMode(shaderClouds);
      // Instead of a blank rectangle you could render here
      // a render texture of the full screen used to do screen
      // scaling (slight adjustment to shader would be required
      // to actually pay attention to the colour!)
      DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
  EndShaderMode();
  //DrawTexturePro(mountain,(Rectangle){0,0,mountain.width,mountain.height},(Rectangle){
  //  0,300,300,300},origin,0,WHITE);
  
  DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
  //DrawRectangle(100, 100, 100, 300, RED);
  //DrawRectangleV((Vector2){300, 200}, (Vector2){100, 30}, BLUE);
  //sysDraw.update(&e);
  ecs_run_systems(ecs, ECS_SYSTEM_CLOUD_UPDATE);
  EndDrawing();
  //----------------------------------------------------------------------------------
}

