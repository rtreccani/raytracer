#include <graphics.h>
#include <stdio.h>

void draw_dot(char symbol, colors_t col, float brightness){
    if(brightness > 0.5){
        fputs("\e[1m", stdout);
    }
    switch(col){
        case RED :
            fputs("\x1B[31m", stdout);
            break;
        case GREEN :
            fputs("\x1B[32m", stdout);
            break;
        case YELLOW :
            fputs("\x1B[33m", stdout);
            break;
        case BLUE :
            fputs("\x1B[34m", stdout);
            break;
        case MAGENTA :
            fputs("\x1B[35m", stdout);
            break;
        case CYAN :
            fputs("\x1B[36m", stdout);
            break;
        case WHITE :
            fputs("\x1B[37m", stdout);
            break;
        default:
            fputs("\x1B[0m", stdout);
            break;
    }
    putc(symbol, stdout);
    fputs("\x1B[0m", stdout);
}