#include <vec_maths.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void vec_rotate_x(vec_3_t* vec_in, vec_3_t* vec_out, float rot){
    vec_out->x = vec_in->x;
    vec_out->y = vec_in->y*cos(rot) - vec_in->z * sin(rot);
    vec_out->z = vec_in->z*cos(rot) + vec_in->y * sin(rot);
}

void vec_rotate_y(vec_3_t* vec_in, vec_3_t* vec_out, float rot){
    vec_out->x = vec_in->x * cos(rot) + vec_in->z * sin(rot);
    vec_out->y = vec_in->y;
    vec_out->z = vec_in->z * cos(rot) - vec_in->x * sin(rot);
}

void vec_rotate_z(vec_3_t* vec_in, vec_3_t* vec_out, float rot){
    vec_out->x = vec_in->x * cos(rot) - vec_in->y * sin(rot);
    vec_out->y = vec_in->y * cos(rot) + vec_in->x * sin(rot);
    vec_out->z = vec_in->z;
}

void vec_rotate_3d(vec_3_t* vec_in, vec_3_t* vec_out, float a, float b, float c){
    vec_out->x = vec_in->x * cos(b) * cos(c) - vec_in->y * cos(b) * sin(c) + vec_in->z * sin(b);
    vec_out->y = vec_in->x * (sin(a) * sin(b) * cos(c) + cos(a) * sin(c)) + vec_in->y * (cos(a) * cos(c) - sin(a) * sin(b) * sin(c)) - vec_in->z * (sin(a) * cos(b));
    vec_out->z = vec_in->x * (sin(a) * sin(c) - cos(a) * sin(b) * cos(c)) + vec_in->y * (cos(a) * sin(b) * sin(c) + sin(a) * cos(c)) + vec_in->z * (cos(a) * cos(b));
}

void printvec3d(vec_3_t* vec){
    printf("{%f, %f, %f}\n", vec->x, vec->y, vec->z);
}


void vec3_add(vec_3_t* p1, vec_3_t* p2, vec_3_t* p_return){
    p_return->x = p1->x + p2->x;
    p_return->y = p1->y + p2->y;
    p_return->z = p1->z + p2->z;
}


void vec3_subtract(vec_3_t* p1, vec_3_t* p2, vec_3_t* p_return){
    p_return->x = p1->x - p2->x;
    p_return->y = p1->y - p2->y;
    p_return->z = p1->z - p2->z;
}

void vec3_cross_product(vec_3_t* p1, vec_3_t* p2, vec_3_t* p_return){
    p_return->x = p1->y * p2->z - p1->z * p2->y;
    p_return->y = p1->z * p2->x - p1->x * p2->z;
    p_return->z = p1->x * p2->y - p1->y * p2->x;
}

float vec3_dot_product(vec_3_t* p1, vec_3_t* p2){
    return ((p1->x * p2->x) + (p1->y * p2->y) + (p1->z * p2->z));
}

void vec3_scalar_mult(vec_3_t* p1, float scalar, vec_3_t* p_return){
    p_return->x = p1->x * scalar;
    p_return->y = p1->y * scalar;
    p_return->z = p1->z * scalar;
}

float vec3_magnitude(vec_3_t* p_vec){
    float mag = pow((p_vec->x), 2) + pow((p_vec->y), 2) + pow((p_vec->z), 2);
    return sqrt(mag);
}

/* return: length til intersect. -1 if doesn't intersect */
float doesRayIntersectTris(tris_t* p_tris, ray_t* p_ray){
    float ray_dist = -1;

    /* compute normal of plane across tris */
    vec_3_t p0p1, p1p2, p2p0, p0p2, normal;
    vec3_subtract(p_tris->p_points[1], p_tris->p_points[0], &p0p1);
    vec3_subtract(p_tris->p_points[2], p_tris->p_points[0], &p0p2);
    vec3_subtract(p_tris->p_points[2], p_tris->p_points[1], &p1p2);
    vec3_subtract(p_tris->p_points[0], p_tris->p_points[2], &p2p0);

    vec3_cross_product(&p0p1, &p0p2, &normal);
    float normal_mag = vec3_magnitude(&normal);

    /*check if ray and tris are parallel */
    float n_dot_ray_dir = vec3_dot_product(&normal, p_ray->p_dir);
    if(abs(n_dot_ray_dir) < 0.0001){
        return -1.0;
    }

    float d = -1 * vec3_dot_product(&normal, p_tris->p_points[0]);
    float t = -1 * ((vec3_dot_product(&normal, p_ray->p_origin) + d) / n_dot_ray_dir);

    if(t < 0){
        return -1.0;
    }

    vec_3_t dir_scaled, P, C, vP0, vP1, vP2;
    /* calculate P */
    vec3_scalar_mult(p_ray->p_dir, t, &dir_scaled);
    vec3_add(p_ray->p_origin, &dir_scaled, &P);

    //edge 0
    vec3_subtract(&P, p_tris->p_points[0], &vP0);
    vec3_cross_product(&p0p1, &vP0, &C);
    if(vec3_dot_product(&normal, &C) < 0){
        return -1.0;
    }

    //edge 1
    vec3_subtract(&P, p_tris->p_points[1], &vP1);
    vec3_cross_product(&p1p2, &vP1, &C);
    if(vec3_dot_product(&normal, &C) < 0){
        return -1.0;
    }

    //edge 2
    vec3_subtract(&P, p_tris->p_points[2], &vP2);
    vec3_cross_product(&p2p0, &vP2, &C);
    if(vec3_dot_product(&normal, &C) < 0){
        return -1.0;
    }
    return t;
}

void vec3_reflection(vec_3_t* vec_in, vec_3_t* vec_out, tris_t* p_tris){
    /* compute normal of plane across tris */
    vec_3_t p0p1, p1p2, p2p0, p0p2, normal, normal_normalised;
    vec3_subtract(p_tris->p_points[1], p_tris->p_points[0], &p0p1);
    vec3_subtract(p_tris->p_points[2], p_tris->p_points[0], &p0p2);
    vec3_subtract(p_tris->p_points[2], p_tris->p_points[1], &p1p2);
    vec3_subtract(p_tris->p_points[0], p_tris->p_points[2], &p2p0);

    vec3_cross_product(&p0p1, &p0p2, &normal);
    float normal_mag = vec3_magnitude(&normal);

    vec3_scalar_mult(&normal, 1/normal_mag, &normal_normalised);

    vec_3_t scaled_diff;
    float diff_scale = vec3_dot_product(vec_in, &normal);
    vec3_scalar_mult(&normal, diff_scale, &scaled_diff);
    vec3_subtract(vec_in, &scaled_diff, vec_out);
}
