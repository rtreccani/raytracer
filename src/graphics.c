#include <graphics.h>
#include <stdio.h>

void draw_pixel(int x, int y, int dx, int dy, col_t col){
    S2D_DrawQuad(dx*x, dy*y, col.r, col.g, col.b, 1,
                 dx*x, dy*y+dy, col.r, col.g, col.b, 1,
                 dx*x+dx, dy*y+dy, col.r, col.g, col.b, 1,
                 dx*x+dx, dy*y, col.r, col.g, col.b, 1);
}