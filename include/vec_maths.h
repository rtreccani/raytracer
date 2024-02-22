#ifndef __VEC_MATHS_H
#define __VEC_MATHS_H

#include <graphics.h>

typedef struct{
    float x;
    float y;
    float z;
} vec_3_t;

typedef struct{
    vec_3_t* p_points[3];
    col_t tris_col;
    surface_t tris_surf;
} tris_t;

typedef struct{
    vec_3_t p_origin;
    vec_3_t p_dir;
} ray_t;

typedef struct{
    vec_3_t p_origin;
    vec_3_t p_dir;
} ray_t2;


vec_3_t vec_rotate_x(vec_3_t vec_in, float x_rot_val);
vec_3_t vec_rotate_y(vec_3_t vec_in, float y_rot_val);
vec_3_t vec_rotate_z(vec_3_t vec_in, float z_rot_val);
vec_3_t vec_rotate_3d(vec_3_t vec_in, float a, float b, float c);
vec_3_t vec_get_reflection(vec_3_t vec_in, tris_t* tris);

void printvec3d(vec_3_t vec);
float doesRayIntersectTris(tris_t p_tris, ray_t p_ray);
vec_3_t vec3_add(vec_3_t p1, vec_3_t p2);
vec_3_t vec3_subtract(vec_3_t p1, vec_3_t p2);
vec_3_t vec3_cross_product(vec_3_t p1, vec_3_t p2);
float vec3_dot_product(vec_3_t p1, vec_3_t p2);
vec_3_t vec3_scalar_mult(vec_3_t p1, float scalar);
float vec3_magnitude(vec_3_t p_vec);

#endif