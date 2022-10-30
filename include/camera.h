#ifndef __CAMERA_H
#define __CAMERA_H

#include <vec_maths.h>

typedef struct{
    ray_t2 ray;
    int res_x;
    int res_y;
    int sensor_size;
    int focal_length;
} camera_t;

#endif