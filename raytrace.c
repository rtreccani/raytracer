#include <stdio.h>
#include <vec_maths.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <camera.h>
#include <graphics.h>

#define FOCAL_LENGTH 6.0f
#define CAMERA_RES_X 120
#define CAMERA_RES_Y 75
#define CAMERA_SENSOR_SIZE 2.0f

vec_3_t camera_position = {
    .x = 0.0,
    .y = 0.0,
    .z = -10.0};

camera_t cam = {
    .focal_length = 3,
    .res_x = 120,
    .res_y = 75,
    .sensor_size = 2,
    .ray = {
        .p_origin = {0, 0, -10},
        .p_dir = {0, 0, 1},
    },
};

vec_3_t light_points[4] = {
    {5, -2, -2},
    {5, -2, 2},
    {5, 2, -2},
    {5, 2, 2},
};

vec_3_t orig_cube_points[8] = {
    {.x = -1, .y = -1, .z = -1},
    {.x = -1, .y = -1, .z = 1},
    {.x = -1, .y = 1, .z = -1},
    {.x = -1, .y = 1, .z = 1},
    {.x = 1, .y = -1, .z = -1},
    {.x = 1, .y = -1, .z = 1},
    {.x = 1, .y = 1, .z = -1},
    {.x = 1, .y = 1, .z = 1},
};

vec_3_t cube_points[8];

tris_t trises[12] = {
    {{&cube_points[0], &cube_points[2], &cube_points[4]}, '$', RED, MATTE}, // front surface
    {{&cube_points[2], &cube_points[4], &cube_points[6]}, '$', RED, MATTE},

    {{&cube_points[0], &cube_points[1], &cube_points[3]}, '#', GREEN, MATTE}, // left surface
    {{&cube_points[0], &cube_points[2], &cube_points[3]}, '#', GREEN, MATTE},

    {{&cube_points[0], &cube_points[1], &cube_points[4]}, '@', YELLOW, MATTE}, // bottom surface
    {{&cube_points[1], &cube_points[4], &cube_points[5]}, '@', YELLOW, MATTE},

    {{&cube_points[2], &cube_points[3], &cube_points[6]}, '%', BLUE, MATTE}, // top surface
    {{&cube_points[3], &cube_points[6], &cube_points[7]}, '%', BLUE, MATTE},

    {{&cube_points[4], &cube_points[6], &cube_points[7]}, '&', MAGENTA, MATTE}, // right surface
    {{&cube_points[4], &cube_points[5], &cube_points[7]}, '&', MAGENTA, MATTE},

    {{&cube_points[1], &cube_points[3], &cube_points[5]}, 'B', CYAN, MATTE}, // back surface
    {{&cube_points[3], &cube_points[5], &cube_points[7]}, 'B', CYAN, MATTE},
};

ray_t construct_camera_ray(int x_pos, int y_pos)
{
    ray_t ret_ray;
    ret_ray.p_origin = &camera_position;
    ret_ray.p_dir->z = FOCAL_LENGTH;
    ret_ray.p_dir->x = camera_position.x - ((x_pos - (CAMERA_RES_X / 2)) * (CAMERA_SENSOR_SIZE / CAMERA_RES_X));
    ret_ray.p_dir->y = camera_position.y - ((y_pos - (CAMERA_RES_Y / 2)) * (CAMERA_SENSOR_SIZE / CAMERA_RES_Y));
    return ret_ray;
}

beam_eval_t find_closest_tris_shade(ray_t ray, tris_t *trises, int tris_length)
{
    beam_eval_t best_guess;
    best_guess.col = BLACK;
    best_guess.shade = ' ';
    float closest_dist = 100000000;
    for (int i = 0; i < tris_length; i++)
    {
        float tris_distance = doesRayIntersectTris(trises[i], ray);
        if (tris_distance > 0 && tris_distance < closest_dist)
        {
            closest_dist = tris_distance;
            best_guess.shade = trises[i].tris_shade;
            best_guess.col = trises[i].tris_col;
            best_guess.lum = 1;
            best_guess.surf = trises[i].tris_surf;
        }
    }
    return best_guess;
}

float find_closest_tris(ray_t ray, tris_t *trises, int tris_length, tris_t* result_tris)
{
    int best_guess = 0;
    float closest_dist = 100000000;
    int tris_found = 0;
    for (int i = 0; i < tris_length; i++)
    {
        tris_found = 1;
        float tris_distance = doesRayIntersectTris(trises[i], ray);
        if (tris_distance > 0 && tris_distance < closest_dist)
        {
            best_guess = i;
        }
    }
    if(tris_found){
        return closest_dist;
    } else {
        return -1.0;
    }
    result_tris = &trises[best_guess];
}

beam_eval_t evaluate_ray(ray_t ray, tris_t *trises, int tris_length, int recurse_depth, vec_3_t* vector_source)
{
    beam_eval_t beam_result;
    tris_t *p_nearest_tris;
    float distance_to_obj = find_closest_tris(ray, trises, tris_length, p_nearest_tris);
    switch(p_nearest_tris->tris_surf){
        case EMISSIVE: 
            beam_result.col = p_nearest_tris->tris_col;
            beam_result.lum = 1;
            break;
        // case GLOSS:
            // vec_3_t reflection_position, inbound_beam_normalised, inbound_beam_scaled_to_reflection, outbound_beam;
            // ray_t new_ray;
            // inbound_beam_normalised = vec3_scalar_mult(*ray->p_dir, 1/vec3_magnitude(ray->p_dir));
            // inbound_beam_scaled_to_reflection = vec3_scalar_mult(inbound_beam_normalised, distance_to_obj);
            // reflection_position = vec3_add(*ray->p_origin, inbound_beam_scaled_to_reflection);
            // new_ray.p_origin = &reflection_position;
            // vec3_reflection(&inbound_beam_normalised, &outbound_beam, p_nearest_tris);
            // new_ray.p_dir = &outbound_beam;
            // return evaluate_ray()// next step
    }
}



beam_eval_t camera_buffer[CAMERA_RES_Y][CAMERA_RES_X];

void render(camera_t *cam)
{
    vec_3_t camera_ray_dir = {
        .x = 0,
        .y = 0,
        .z = 1,
    };
    ray_t camera_ray = {
        .p_origin = &camera_position,
        .p_dir = &camera_ray_dir,
    };

    for (int y = 0; y < CAMERA_RES_Y; y++)
    {
        for (int x = 0; x < CAMERA_RES_X; x++)
        {
            camera_ray = construct_camera_ray(x, y);
            camera_buffer[y][x] = find_closest_tris_shade(camera_ray, trises, 12);
        }
    }
    system("clear");
    for (int y = 0; y < CAMERA_RES_Y; y++)
    {
        for (int x = 0; x < CAMERA_RES_X; x++)
        {
            draw_dot(camera_buffer[y][x].shade, camera_buffer[y][x].col, camera_buffer[y][x].lum);
        }
        putc('\n', stdout);
    }
}

int main(int argc, void **argv)
{
    vec_3_t cube_rotation = {3.14 / 4, 3.14 / 4, 0.07};

    while (1 == 1)
    {
        for (int i = 0; i < 8; i++)
        {
            cube_points[i] = vec_rotate_3d(orig_cube_points[i], cube_rotation.x, cube_rotation.y, cube_rotation.z);
        }
        render(&cam);
        usleep(50000);
        cube_rotation.x += 3.14 / 30;
        cube_rotation.y += 3.14 / 26;
        cube_rotation.z += 3.14 / 35;
    }
}
