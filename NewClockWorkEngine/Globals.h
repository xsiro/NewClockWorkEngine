#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI
#define BTOGB (1073741824.0f)
#define KBTOMB 1024.0f //To GB: (1048576.0f)
#define BTOMB (1048576.0f)


typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true
#define TITLE "NewClockWorkEngine"
#define ORGANIZATION "CITM - UPC"


#define MAXFPSDISPLAY 100 //TODO make this a configurable variable


// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
    {                              \
    if( x != NULL )              \
	    {                            \
      delete[] x;                \
	  x = NULL;                    \
	    }                            \
                              \
    }

class Application;

extern Application* App;

//TODO change these 2 defines to const Color()?
#define SELECTED_COLOR (0.75f, 0.25f,0.0f,1.0f) //all of selected objs
#define FOCUSED_COLOR (1.0f, 0.5f, 0.0f, 1.0f) //focused color of last selected obj

//define paths
#define LIB_PATH "Library/"
#define MESH_PATH "Library/Meshes/"
#define MATERIAL_PATH "Library/Materials/"
#define TEXTURE_PATH "Library/Textures/"
#define SCENE_PATH "Library/Scenes/"
#define MODEL_PATH "Library/Models/"


