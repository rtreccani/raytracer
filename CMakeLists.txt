cmake_minimum_required(VERSION 3.10)
project("raytracer")

find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_library(SIMPLE2D_LIB libsimple2d.a /usr/local/lib)

add_executable(raytracer src/vec_maths.c src/graphics.c raytrace.c )
target_include_directories(raytracer PUBLIC include ${SDL2_INCLUDE_DIRS})

set(CMAKE_BUILD_TYPE Debug)

target_link_libraries(raytracer m ${SIMPLE2D_LIB} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} SDL2 SDL2_mixer SDL2_image SDL2_ttf)