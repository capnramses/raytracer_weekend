/*
                         Anton's 3d Maths Library
                             Clang C99 branch

                            Dr Anton Gerdelan
                         Trinity College Dublin
                                Ireland
                          <gerdela@scss.tcd.ie>
                        this version: 12 Feb 2016

This branch only works with the Clang C compiler as it uses Clang's OpenCL
vector data type extensions.
For the C++ version see the .hpp file.

  * Commonly-used maths structures and functions, resembling GLSL.
  * Simple-as-possible.
  * Function names have suffix to denote type as C has no function overloading.
  * vec2, vec3, vec4, versor components can be accessed with GLSL style
    "swizzling" e.g. V.xyzw or V.zy, or by array-like access i.e. V[2]
  * a versor is the proper name for a unit quaternion.

#include this header file.
In C we need to link math.h's libm: "-lm"
*/
#pragma once

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h> // memset, memcpy

// C99 removed M_PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
// const used to convert degrees into radians
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444
#define ONE_RAD_IN_DEG 360.0 / (2.0 * M_PI) //57.2957795

typedef float vec4 __attribute__((ext_vector_type(4))); // OpenCL style
typedef float vec3 __attribute__((ext_vector_type(3)));
typedef float vec2 __attribute__((ext_vector_type(2)));
typedef float versor __attribute__((ext_vector_type(4)));
typedef float mat4 __attribute__((ext_vector_type(16)));
typedef float mat3 __attribute__((ext_vector_type(9)));

void print_vec2 (vec2 v);
void print_vec3 (vec3 v);
void print_vec4 (vec4 v);
void print_mat4 (mat4 m);
void print_quat (versor q);

// vector functions -- geometric
// vector magnitude
float length_vec3 (vec3 v);
// squared vector length
float length2_vec3 (vec3 v);
//
vec3 normalise_vec3 (vec3 v);
// dot product
float dot_vec3 (vec3 a, vec3 b);
// cross product
vec3 cross_vec3 (vec3 a, vec3 b);
// an arbitrary -Z == 0 degrees, -X = 90 degrees etc. thing
float vec3_to_heading (vec3 d);
// reverse of the above
vec3 heading_to_vec3 (float degrees);

mat4 zero_mat4 ();
mat4 identity_mat4 ();
mat4 mat4_mul_mat4 (mat4 a, mat4 b);
vec4 mat4_mul_vec4 (mat4 m, vec4 v);
// determinant
float det (mat4 mm);
mat4 inverse (mat4 mm);
mat4 transpose (mat4 mm);

// matrix functions -- affine functions
mat4 translate_mat4 (vec3 v);
mat4 rot_x_deg_mat4 (float deg);
mat4 rot_y_deg_mat4 (float deg);
mat4 rot_z_deg_mat4 (float deg);
mat4 scale_mat4 (vec3 v);

// matrix functions -- camera functions
mat4 look_at (vec3 cam_pos, vec3 targ_pos, vec3 up);
mat4 perspective (float fovy, float aspect, float near, float far);

// quaternion functions -- geometric
versor quat_from_axis_rad (float rad, vec3 axis);
versor quat_from_axis_deg (float deg, vec3 axis);
// note: NOTE the same as the built-in q * q component-wise vector function
versor quat_mul_quat (versor a, versor b);
// same as q + q but does normalisation check
versor add_quat_quat (versor a, versor b);
mat4 quat_to_mat4 (versor q);
float dot_quat (versor q, versor r);
versor normalise_quat (versor q);

// quaternion functions -- interpolation
versor slerp_quat (versor q, versor r, float t);

