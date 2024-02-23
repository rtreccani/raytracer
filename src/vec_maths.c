#include <vec_maths.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

vec_3_t vec_rotate_x(vec_3_t vec_in, float rot){
    vec_3_t vec_out;
    vec_out.x = vec_in.x;
    vec_out.y = vec_in.y*cos(rot) - vec_in.z * sin(rot);
    vec_out.z = vec_in.z*cos(rot) + vec_in.y * sin(rot);
    return vec_out;
}

vec_3_t vec_rotate_y(vec_3_t vec_in, float rot){
    vec_3_t vec_out;
    vec_out.x = vec_in.x * cos(rot) + vec_in.z * sin(rot);
    vec_out.y = vec_in.y;
    vec_out.z = vec_in.z * cos(rot) - vec_in.x * sin(rot);
    return vec_out;
}

vec_3_t vec_rotate_z(vec_3_t vec_in, float rot){
    vec_3_t vec_out;
    vec_out.x = vec_in.x * cos(rot) - vec_in.y * sin(rot);
    vec_out.y = vec_in.y * cos(rot) + vec_in.x * sin(rot);
    vec_out.z = vec_in.z;
    return vec_out;
}

vec_3_t vec_rotate_3d(vec_3_t vec_in, float a, float b, float c){
    vec_3_t vec_out;
    vec_out.x = vec_in.x * cos(b) * cos(c) - vec_in.y * cos(b) * sin(c) + vec_in.z * sin(b);
    vec_out.y = vec_in.x * (sin(a) * sin(b) * cos(c) + cos(a) * sin(c)) + vec_in.y * (cos(a) * cos(c) - sin(a) * sin(b) * sin(c)) - vec_in.z * (sin(a) * cos(b));
    vec_out.z = vec_in.x * (sin(a) * sin(c) - cos(a) * sin(b) * cos(c)) + vec_in.y * (cos(a) * sin(b) * sin(c) + sin(a) * cos(c)) + vec_in.z * (cos(a) * cos(b));
    return vec_out;
}

void printvec3d(vec_3_t vec){
    printf("{%f, %f, %f}\n", vec.x, vec.y, vec.z);
}


vec_3_t vec3_add(vec_3_t p1, vec_3_t p2){
    vec_3_t ret;
    ret.x = p1.x + p2.x;
    ret.y = p1.y + p2.y;
    ret.z = p1.z + p2.z;
    return ret;
}


vec_3_t vec3_subtract(vec_3_t p1, vec_3_t p2){
    vec_3_t ret;
    ret.x = p1.x - p2.x;
    ret.y = p1.y - p2.y;
    ret.z = p1.z - p2.z;
    return ret;
}

vec_3_t vec3_cross_product(vec_3_t p1, vec_3_t p2){
    vec_3_t ret;
    ret.x = p1.y * p2.z - p1.z * p2.y;
    ret.y = p1.z * p2.x - p1.x * p2.z;
    ret.z = p1.x * p2.y - p1.y * p2.x;
    return ret;
}

float vec3_dot_product(vec_3_t p1, vec_3_t p2){
    return ((p1.x * p2.x) + (p1.y * p2.y) + (p1.z * p2.z));
}

vec_3_t vec3_scalar_mult(vec_3_t p1, float scalar){
    vec_3_t ret;
    ret.x = p1.x * scalar;
    ret.y = p1.y * scalar;
    ret.z = p1.z * scalar;
    return ret;
}

float vec3_magnitude(vec_3_t vec){
    float mag = pow((vec.x), 2) + pow((vec.y), 2) + pow((vec.z), 2);
    return sqrt(mag);
}

/* return: length til intersect. -1 if doesn't intersect */
float doesRayIntersectTris(tris_t tris, ray_t ray){
    // ray.dir = vec3_scalar_mult(ray.dir, 1/vec3_magnitude(ray.dir));

    /* compute normal of plane across tris */
    vec_3_t p0p1 = vec3_subtract(*tris.p_points[1], *tris.p_points[0]);
    vec_3_t p0p2 = vec3_subtract(*tris.p_points[2], *tris.p_points[0]);
    vec_3_t p1p2 = vec3_subtract(*tris.p_points[2], *tris.p_points[1]);
    vec_3_t p2p0 = vec3_subtract(*tris.p_points[0], *tris.p_points[2]);

    vec_3_t normal =vec3_cross_product(p0p1, p0p2);
    float normal_mag = vec3_magnitude(normal);

    /*check if ray and tris are parallel */
    float n_dot_ray_dir = vec3_dot_product(normal, ray.dir);
    if(abs(n_dot_ray_dir) < EPSILON){
        return -1.0;
    }

    float d = -1 * vec3_dot_product(normal, *tris.p_points[0]);
    float t = -1 * ((vec3_dot_product(normal, ray.origin) + d) / n_dot_ray_dir);

    if(t < EPSILON){
        return -1.0;
    }

    /* calculate P */
    vec_3_t P = vec3_add(ray.origin, vec3_scalar_mult(ray.dir, t));

    //edge 0
    vec_3_t C = vec3_cross_product(p0p1, vec3_subtract(P, *tris.p_points[0]));
    if(vec3_dot_product(normal, C) < -EPSILON){
        return -1.0;
    }

    //edge 1
    C = vec3_cross_product(p1p2, vec3_subtract(P, *tris.p_points[1]));
    if(vec3_dot_product(normal, C) < -EPSILON){
        return -1.0;
    }

    //edge 2
    C = vec3_cross_product(p2p0, vec3_subtract(P, *tris.p_points[2]));
    if(vec3_dot_product(normal, C) < -EPSILON){
        return -1.0;
    }
    return t;
}

vec_3_t vec_get_reflection(vec_3_t vec_in, tris_t tris){
    vec_3_t vec_out;
    /* compute normal of plane across tris */
    vec_3_t p0p1, p1p2, p2p0, p0p2, normal, normal_normalised;
    p0p1 = vec3_subtract(*tris.p_points[1], *tris.p_points[0]);
    p0p2 = vec3_subtract(*tris.p_points[2], *tris.p_points[0]);
    p1p2 = vec3_subtract(*tris.p_points[2], *tris.p_points[1]);
    p2p0 = vec3_subtract(*tris.p_points[0], *tris.p_points[2]);

    normal = vec3_cross_product(p0p1, p0p2);
    normal_normalised = vec3_scalar_mult(normal, 1/vec3_magnitude(normal));

    /* don't ask */
    vec_3_t scaled_diff = vec3_scalar_mult(vec3_scalar_mult(normal_normalised, vec3_dot_product(vec_in, normal_normalised)), 2);
    vec_out = vec3_subtract(vec_in, scaled_diff);
    return vec_out;
}
