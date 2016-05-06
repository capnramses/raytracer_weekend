#pragma once

#define USEDRAND48
#ifdef USEDRAND48
#define __USE_SVID // drand48
#define _XOPEN_SOURCE
#endif
#include "apg_maths_clang.h"
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define NSAMPLES 50
#define NX 600
#define NY 400
#define MAXTHREADS 64

typedef enum Mat_Type {
	LAMBERTIAN,
	METAL,
	DIELECTRIC
} Mat_Type;

// point at param = Ray.origin + direction * t;
typedef struct Ray { vec3 origin; vec3 direction; float tmax; float tmin; } Ray;
typedef struct Material { Mat_Type type; vec3 albedo; float fuzz; float ior; } Material;
#define NSPHERES 5
typedef struct Sphere { vec3 centre; float radius; Material material; } Sphere;
typedef struct World { Sphere spheres[NSPHERES]; } World;
typedef struct Hit_Record { float t; vec3 p; vec3 n; Material material; } Hit_Record;
vec3 colour_ray (Ray ray, const World* world, int depth);
typedef struct Thread_Data { int x, y; Ray ray; } Thread_Data;

vec3 reflect (vec3 v, vec3 n);
vec3 refract (vec3 v, vec3 n, float ni_over_nt, bool* refracted);

extern unsigned char* img_data;
extern vec3 lower_left_corner; // of screen
extern vec3 horizontal; // width of screen
extern vec3 vertical; // height of screen
extern World world;
extern Thread_Data thread_data[MAXTHREADS];

