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

typedef struct MovingCloud{
    Vector2 vel;
    float inner;
    float radius;
    bool isActive;
    Vector2 sizes[4];
    Vector2 positions[4];
} MovingCloud;

typedef struct TintedSprite {
  Color fromColor, toColor;
  Vector2 fromPos, toPos;
  Vector2 fromSize,toSize;
  float progress;
} TintedSprite;


typedef struct SpriteRender{
  Texture2D *sprite;
  Rectangle sourceRect;
  Rectangle targetRect;
  
} SpriteRender;


Ecs *ecs;
EcsEnt e;
EcsEnt sunEntity; 
EcsEnt mountainEntity;
EcsEnt heartScoreEntity;
Shader shaderClouds;
Shader shaderSun;
Texture2D mountain;
Texture2D sun; 
Texture2D heartTexture; 

unsigned int cloudShaderTintColorPos;

typedef enum
{
    COMPONENT_TRANSFORM,
    COMPONENT_VELOCITY,
    COMPONENT_SPRITE,
    COMPONENT_TINTED_SPRITE,
    COMPONENT_HEART_SCORE_BAR,
    COMPONENT_MISSLE,
    COMPONENT_CLOUD_SPOT,
    COMPONENT_COUNT
} ComponentType;

 

Color interpolate(Color a, Color b, float t)
{
    // 0.0 <= t <= 1.0
    return (Color){
        a.r + (b.r - a.r) * t,
        a.g + (b.g - a.g) * t,
        a.b + (b.b - a.b) * t,
        a.a + (b.a - a.a) * t
    };
}

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

#define SPRITE_RENDER_SYSTEM_MASK \
ECS_MASK(3, COMPONENT_TRANSFORM,COMPONENT_SPRITE,COMPONENT_TINTED_SPRITE)
void 
sprite_render_system(Ecs *ecs)
{
    for (int i = 0; i < ecs_for_count(ecs); i++)
    {
        EcsEnt e = ecs_get_ent(ecs, i);
        if (ecs_ent_has_mask(ecs, e, SPRITE_RENDER_SYSTEM_MASK))
        {
          TintedSprite *tintC   = ecs_ent_get_component(ecs, e, COMPONENT_TINTED_SPRITE);
          if(tintC->progress<1.0){
            //update
            tintC->progress+=0.01*GetFrameTime();
          }
          CTransform *transformC   = ecs_ent_get_component(ecs, e, COMPONENT_TRANSFORM);
          SpriteRender *s   = ecs_ent_get_component(ecs, e, COMPONENT_SPRITE);
          Vector2 new_p = Vector2Lerp(tintC->fromPos,tintC->toPos,tintC->progress);
          transformC->x = new_p.x;
          transformC->y = new_p.y;
          Color c = interpolate(tintC->fromColor,tintC->toColor,tintC->progress);
          float sizeW = Lerp(tintC->fromSize.x,tintC->toSize.x,tintC->progress);
          float sizeH = Lerp(tintC->fromSize.x,tintC->toSize.x,tintC->progress);
          SetShaderValue(shaderClouds, cloudShaderTintColorPos,&c, SHADER_UNIFORM_VEC3);

          DrawRectangle(0,0,screenWidth,screenWidth,c);

          DrawTexturePro(*s->sprite,
            s->sourceRect,
            (Rectangle){transformC->x,transformC->y,sizeW,sizeH},
            (Vector2){0,0},
            0,c);
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
            MovingCloud *s   = ecs_ent_get_component(ecs, e, COMPONENT_CLOUD_SPOT);
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

                             
        }
    }
    // draw the heart score 
    SpriteRender *heartRender = ecs_ent_get_component(ecs,heartScoreEntity,COMPONENT_SPRITE);
    DrawTexturePro(*heartRender->sprite,
    heartRender->sourceRect,
    heartRender->targetRect,
    (Vector2){0,0},
    0,WHITE);       
}

void 
register_components(Ecs *ecs)
{
    //Ecs, component index, component pool size, size of component, and component free func
    ecs_register_component(ecs, COMPONENT_TRANSFORM, 1000, sizeof(CTransform), NULL);
    ecs_register_component(ecs, COMPONENT_CLOUD_SPOT, 1000, sizeof(MovingCloud), NULL);
    ecs_register_component(ecs, COMPONENT_SPRITE, 1000, sizeof(SpriteRender), NULL);
    ecs_register_component(ecs, COMPONENT_TINTED_SPRITE, 1000, sizeof(TintedSprite), NULL);


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
    ecs_register_system(ecs, sprite_render_system, ECS_SYSTEM_SPRITE_RENDER);
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
  heartTexture = LoadTexture("../../resources/heart.png");

  iTimeLocation = GetShaderLocation(shaderClouds, "iTime");
  cloudShaderTintColorPos = GetShaderLocation(shaderClouds, "tintColor");



  //Max entities, component count, system_count
  ecs = ecs_make(1000, COMPONENT_COUNT, 3);
  register_components(ecs);
  register_systems(ecs);


  sunEntity = ecs_ent_make(ecs);

  CTransform sunTransform = (CTransform){50, 50};
  SpriteRender r; 
  r.sourceRect = (Rectangle){0,0,sun.width,sun.height};
  r.targetRect = (Rectangle){10,100,100,100};
  r.sprite = &sun;

  ecs_ent_add_component(ecs, sunEntity, COMPONENT_TRANSFORM, &sunTransform);    
  ecs_ent_add_component(ecs, sunEntity, COMPONENT_SPRITE, &r);
  TintedSprite ts; 
  ts.fromColor = (Color){255,255,255,255};
  ts.toColor = (Color){255,1,1,255};
  ts.fromPos = (Vector2){50,50};
  ts.toPos = (Vector2){550,200};
  ts.fromSize = (Vector2){100,100};
  ts.toSize = (Vector2){400,400};
  ts.progress = 0;
  ecs_ent_add_component(ecs, sunEntity, COMPONENT_TINTED_SPRITE, &ts);

  e = ecs_ent_make(ecs);
  CTransform xform = {0, 0};
  ecs_ent_add_component(ecs, e, COMPONENT_TRANSFORM, &xform);    
  startTime = GetTime();

  heartScoreEntity = ecs_ent_make(ecs);

  CTransform heartTransform = {200, 100};
  ecs_ent_add_component(ecs, e, COMPONENT_TRANSFORM, &heartTransform);    

  SpriteRender heartRenderer; 
  r.sourceRect = (Rectangle){0,0,heartTexture.width,heartTexture.height};
  r.targetRect = (Rectangle){100,200,100,100};
  r.sprite = &heartTexture;
  ecs_ent_add_component(ecs, heartScoreEntity, COMPONENT_SPRITE, &heartRenderer);

  for (int i = 0;i<4;i++){
    EcsEnt cloud = ecs_ent_make(ecs);
    CTransform cloudT = {screenHeight+50, GetRandomValue(100,screenWidth)};
    ecs_ent_add_component(ecs, e, COMPONENT_TRANSFORM, &cloudT);    
    MovingCloud mc;
    mc.inner=2;
    mc.radius=2;
    mc.isActive=true;
    mc.vel=(Vector2){-1,0};
    ecs_ent_add_component(ecs, e, COMPONENT_CLOUD_SPOT, &mc);    

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
  
  ecs_run_systems(ecs, ECS_SYSTEM_SPRITE_RENDER);
  BeginShaderMode(shaderClouds);
      // Instead of a blank rectangle you could render here
      // a render texture of the full screen used to do screen
      // scaling (slight adjustment to shader would be required
      // to actually pay attention to the colour!)
      DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
  EndShaderMode();
  DrawTexturePro(mountain,(Rectangle){0,0,mountain.width,mountain.height},(Rectangle){
  0,300,300,300},origin,0,WHITE);
  
  ecs_run_systems(ecs, ECS_SYSTEM_CLOUD_UPDATE);

  EndDrawing();
  //----------------------------------------------------------------------------------
}

