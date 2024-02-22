#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <simple2d.h>

#define C_RED {1.0, 0.0, 0.0}
#define C_GREEN {0.0, 1.0, 0.0}
#define C_BLUE  {0.0, 0.0, 1.0}
#define C_WHITE {1.0, 1.0, 1.0}
#define C_BLACK {0.0, 0.0, 0.0}
#define C_YELLOW {1.0, 1.0, 0.0}
#define C_MAGENTA {1.0, 0.0, 1.0}
#define C_CYAN {0.0, 1.0, 1.0}

typedef struct{
    float r;
    float g;
    float b;
} col_t;

typedef enum{
    GLOSS,
    MATTE,
    EMISSIVE,
} surface_t;

typedef struct{
    col_t col;
    surface_t surf;
} beam_eval_t;

void draw_pixel(int, int, int, int, col_t);

#endif