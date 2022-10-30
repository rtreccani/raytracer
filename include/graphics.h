#ifndef __GRAPHICS_H
#define __GRAPHICS_H

typedef enum{
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BLACK,
} colors_t;

typedef enum{
    GLOSS,
    MATTE,
    EMISSIVE,
} surface_t;

typedef struct{
    char shade;
    colors_t col;
    surface_t surf;
    float lum;
} beam_eval_t;

void draw_dot(char, colors_t, float);

#endif