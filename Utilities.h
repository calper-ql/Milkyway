//
// Created by calper on 4/19/19.
//

#ifndef MILKYWAYVR_UTILITIES_H
#define MILKYWAYVR_UTILITIES_H

#define NOMINMAX
#include "GL/gl3w.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include <optix.h>
#include <optixu/optixpp_namespace.h>

int initGL();
int initGL3W();
void rt_buffer_to_gl(optix::Buffer buffer, GLuint &gl_tex_id);

#endif //MILKYWAYVR_UTILITIES_H
