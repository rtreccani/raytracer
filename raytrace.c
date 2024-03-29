#include <stdio.h>
#include <vec_maths.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <camera.h>
#include <graphics.h>

#define CAMERA_RES_X 500
#define CAMERA_RES_Y 500
#define FOCAL_LENGTH 2.5f
#define PIXEL_SIZE 2
#define CAMERA_SENSOR_SIZE 2.0f
#define DIFFUSE_SAMPLE_COUNT 100

S2D_Window* win;
col_t camera_buffer[CAMERA_RES_Y][CAMERA_RES_X];

vec_3_t camera_position = {
    .x = 0.0,
    .y = 0.0,
    .z = -20.0};

camera_t cam = {
    .focal_length = FOCAL_LENGTH,
    .res_x = CAMERA_RES_X,
    .res_y = CAMERA_RES_Y,
    .sensor_size = 1.5,
    .ray = {
        .origin = {0, 0, -20},
        .dir = {0, 0, 1},
    },
};

vec_3_t light_points[4] = {
    {3.5, -4, -4},
    {3.5, 4, -4},
    {4.5, -4, 4},
    {4.5, 4, 4},
};

vec_3_t l2_points[4] = {
    {-10, -10, -10},
    {-10, -10, 10},
    {10, -10, -10},
    {10, -10, 10},
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
vec_3_t cube_rotation = {45/6.28, 45/6.28, 45/6.28};

tris_t trises[] = {
    {{&cube_points[0], &cube_points[2], &cube_points[4]}, C_RED, MATTE}, // front surface
    {{&cube_points[2], &cube_points[6], &cube_points[4]}, C_RED, MATTE},

    {{&cube_points[0], &cube_points[1], &cube_points[3]}, C_GREEN, MATTE}, // left surface
    {{&cube_points[0], &cube_points[3], &cube_points[2]}, C_GREEN, MATTE},

    {{&cube_points[0], &cube_points[4], &cube_points[1]}, C_YELLOW, MATTE}, // bottom surface
    {{&cube_points[1], &cube_points[4], &cube_points[5]}, C_YELLOW, MATTE},

    {{&cube_points[2], &cube_points[3], &cube_points[6]}, C_BLUE, MATTE}, // top surface
    {{&cube_points[3], &cube_points[7], &cube_points[6]}, C_BLUE, MATTE},

    {{&cube_points[4], &cube_points[6], &cube_points[7]}, C_MAGENTA, MATTE}, // right surface
    {{&cube_points[4], &cube_points[7], &cube_points[5]}, C_MAGENTA, MATTE},

    {{&cube_points[1], &cube_points[5], &cube_points[3]}, C_CYAN, MATTE}, // back surface
    {{&cube_points[3], &cube_points[5], &cube_points[7]}, C_CYAN, MATTE},

    {{&light_points[0], &light_points[1], &light_points[2]}, C_WHITE, MATTE},
    {{&light_points[1], &light_points[2], &light_points[3]}, C_WHITE, MATTE},

    {{&l2_points[0], &l2_points[1], &l2_points[2]}, C_WHITE, EMISSIVE},
    {{&l2_points[1], &l2_points[2], &l2_points[3]}, C_WHITE, EMISSIVE},
};

ray_t construct_camera_ray(int x_pos, int y_pos)
{
    ray_t ret_ray;
    ret_ray.origin = camera_position;
    ret_ray.dir.z = FOCAL_LENGTH;
    ret_ray.dir.x = camera_position.x - ((x_pos - (CAMERA_RES_X / 2)) * (CAMERA_SENSOR_SIZE / CAMERA_RES_X));
    ret_ray.dir.y = camera_position.y - ((y_pos - (CAMERA_RES_Y / 2)) * (CAMERA_SENSOR_SIZE / CAMERA_RES_Y));
    // ret_ray.dir = vec3_scalar_mult(ret_ray.dir, 1/(vec3_magnitude(ret_ray.dir)));
    return ret_ray;
}

int find_closest_tris(ray_t ray, tris_t *trises, int tris_length){
    int best_guess = -1;
    float closest_dist = 100000000;
    for (int i = 0; i < tris_length; i++)
    {
        float tris_distance = doesRayIntersectTris(trises[i], ray);
        if (tris_distance > 0 && tris_distance < closest_dist)
        {
            closest_dist = tris_distance;
            best_guess = i;
        }
    }
    return best_guess;
}

col_t evaluate_ray(ray_t ray, tris_t *trises, int tris_length, int recurse_depth)
{
    col_t beam_result = C_BLACK;
    if(recurse_depth-- < 0){
        return beam_result;
    }

    int tris_index = find_closest_tris(ray, trises, tris_length);
    if(tris_index < 0){
        return (beam_result);
    }
    ray_t new_ray;
    float distance_to_obj;

    switch(trises[tris_index].tris_surf){
        case EMISSIVE:
            beam_result = trises[tris_index].tris_col;
            break;

        case MATTE:
            vec_3_t normal = vec3_normalize(get_tris_normal(trises[tris_index]));

            ray.dir = vec3_normalize(ray.dir);
            distance_to_obj = doesRayIntersectTris(trises[tris_index], ray);

            new_ray.origin = vec3_add(ray.origin, vec3_scalar_mult(ray.dir, distance_to_obj));

            int samples = 0;
            while(samples < DIFFUSE_SAMPLE_COUNT){
                vec_3_t sample = vec3_normalize(vec3_generate_random_vector());
                if(vec3_dot_product(normal, sample) > 0){
                    new_ray.dir = sample;
                    col_t temp_result = evaluate_ray(new_ray, trises, tris_length, recurse_depth);
                    beam_result.r += (temp_result.r / DIFFUSE_SAMPLE_COUNT);
                    beam_result.g += (temp_result.g / DIFFUSE_SAMPLE_COUNT);
                    beam_result.b += (temp_result.b / DIFFUSE_SAMPLE_COUNT);
                    samples++;
                }
            }
            beam_result.r *= trises[tris_index].tris_col.r;
            beam_result.g *= trises[tris_index].tris_col.g;
            beam_result.b *= trises[tris_index].tris_col.b;
            break;

        case GLOSS:
            ray.dir = vec3_normalize(ray.dir);
            distance_to_obj = doesRayIntersectTris(trises[tris_index], ray);

            new_ray.origin = vec3_add(ray.origin, vec3_scalar_mult(ray.dir, distance_to_obj));

            new_ray.dir = vec_get_reflection(ray.dir, trises[tris_index]);
            beam_result = evaluate_ray(new_ray, trises, tris_length, recurse_depth);// next step
            break;
    }
    return beam_result;
}

void render(camera_t *cam)
{
    for (int y = 0; y < CAMERA_RES_Y; y++)
    {
        for (int x = 0; x < CAMERA_RES_X; x++)
        {
            ray_t camera_ray = construct_camera_ray(x, y);
            camera_buffer[y][x] = evaluate_ray(camera_ray, trises, sizeof(trises)/sizeof(tris_t), 2);
        }
    }
    for (int y = 0; y < CAMERA_RES_Y; y++)
    {
        for (int x = 0; x < CAMERA_RES_X; x++)
        {
            draw_pixel(x, y, PIXEL_SIZE, PIXEL_SIZE, camera_buffer[y][x]);
        }
    }
}

void update_loop(void){
    for (int i = 0; i < 8; i++){
        cube_points[i] = vec_rotate_3d(orig_cube_points[i], cube_rotation.x, cube_rotation.y, cube_rotation.z);
    }
    cube_rotation.x += 3.14 / 340;
    cube_rotation.y += 3.14 / 350;
    cube_rotation.z += 3.14 / 330;
}

void render_loop(void){
    render(&cam);
}

int main(int argc, void **argv)
{
    win = S2D_CreateWindow("Raytracer", CAMERA_RES_X * PIXEL_SIZE, CAMERA_RES_Y * PIXEL_SIZE, update_loop, render_loop, 0);
    win->vsync = true;
    win->fps_cap = 60;
    S2D_Show(win);
}
