#include <stdio.h>
#include <windows.h>

#define U8 unsigned char
#define I8 char
#define U16 unsigned short
#define I16 short
#define U32 unsigned int
#define I32 int
#define U64 unsigned long
#define I64 long int
#define F32 float
#define F64 double
#define BOOL U8

#define RATIO 1.33
#define RES_WIDTH 400
#define RES_HEIGHT 300
#define SIZE 16

BOOL debug = 0;
BOOL fullscreen = 0;
BOOL running = 1;
U32 black_bar = 0;
U8 bar_placement = 1;
F32 resolution = 0;

//Color

#define STD_WHITE 0xFFFFFF
#define STD_RED 0xFF0000
#define STD_GREEN 0x00FF00
#define STD_BLUE 0x0000FF
#define CYAN 0x00FFFF
#define PURPLE 0xFF00FF
#define STD_YELLOW 0xFFFF00

//PICO-8 PALLETE
#define BLACK 0x000000
#define DARK_BLUE 0x1D2B53
#define DARK_PURPLE 0x7E2553
#define DARK_GREEN 0x008751
#define BROWN 0xAB5236
#define DARK_GREY 0x5F574F
#define LIGHT_GREY 0xC2C3C7
#define WHITE 0xFFF1E8
#define RED 0xFF004D
#define ORANGE 0xFFA300
#define YELLOW 0xFFEC27
#define GREEN 0x00E436
#define BLUE 0x29ADFF
#define LAVANDER 0x83769C
#define PINK 0xFF77A8
#define LIGHT_PEACH 0xFFCCAA

//Platform
struct{
    U32 width;
    U32 height;
}typedef WINDOWSIZE;

WINDOWSIZE draw_surface = {0};

struct{
    U32 width;
    U32 height;
    U32 bit_count;
    I32 *memory;//void?
    U32 size;
    BITMAPINFO bitmapinfo;
}typedef BACKBUFFER;

BACKBUFFER back_buffer = {0, 0, 32, NULL, 0, 0};

struct{
    U32 count;
    F32 target;
    F32 cooked;
    F32 delay;
    F32 accum_cooked;
    F32 raw;
    F32 accum_raw;
    LARGE_INTEGER LINT_frequency;
    LARGE_INTEGER LINT_start_time;
    LARGE_INTEGER LINT_end_time;
}typedef FPS;

FPS fps = {0};

//Input
struct{
    BOOL is_down;
    BOOL changed;
}typedef BUTTON;

struct{
    U32 x;
    U32 y;
}typedef MOUSE;

enum{
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    LBUTTON,
    RBUTTON,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ESCAPE,
    
    BUTTON_COUNT,
};

struct{
    MOUSE mouse;
    BUTTON key[BUTTON_COUNT];
}typedef GAMEINPUT;

GAMEINPUT input = {0};

#define pressed(b) input.key[b].is_down == 1 && input.key[b].changed == 1
#define released(b) ((input.key[b].is_down == 1) && (input.key[b].changed == 1))
#define hold(b) input.key[b].is_down == 1
#define button(b) input.key[b]

//Math
#define clamp(min, val, max) if(val < min) val = min; if(val > max) val = max

//Sprite Editor

struct{
    U32 x;
    U32 y;
    U32 color;
}typedef TILEINFO;

TILEINFO colors[16] = {0};
TILEINFO drawboard[64] = {0};
TILEINFO display_mouse = {0};

struct{
    U32 x;
    U32 y;
    TILEINFO pixel[64];
}typedef MAPINFO;

MAPINFO sprite_map[80] = {0};

U32 highlight_x;
U32 highlight_y;

U32 tile_num = 0;