#pragma once

#ifdef USEDRAND48
#define __USE_SVID // drand48
#endif
#include "apg_maths_clang.h"
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define NSAMPLES 100
#define NX 800
#define NY 400
#define MAXTHREADS 64

// point at param = Ray.origin + direction * t;
struct Ray { vec3 origin; vec3 direction; float tmax; float tmin; };
typedef struct Ray Ray;
struct Material { int type; vec3 albedo; float fuzz; float ior; };
typedef struct Material Material;
#define NSPHERES 4
struct Sphere { vec3 centre; float radius; Material material; };
typedef struct Sphere Sphere;
struct World { Sphere spheres[NSPHERES]; };
typedef struct World World;
struct Hit_Record { float t; vec3 p; vec3 n; Material material; };
typedef struct Hit_Record Hit_Record;
struct Thread_Data { int x, y; Ray ray; };
typedef struct Thread_Data Thread_Data;

inline double myrand ();

double myrand () {
#ifdef USEDRAND48
	return drand48 ();
#else
	return (double)rand () / (double)RAND_MAX;
#endif
}

inline vec3 reflect (vec3 v, vec3 n);
inline vec3 refract (vec3 v, vec3 n, float ni_over_nt, bool* refracted);
inline vec3 rand_in_unit_sphere ();
vec3 colour_ray (Ray ray, const World* world, int depth);

extern unsigned char* img_data;
extern vec3 lower_left_corner; // of screen
extern vec3 horizontal; // width of screen
extern vec3 vertical; // height of screen
extern World world;
extern Thread_Data thread_data[MAXTHREADS];