#include "apg_maths_clang.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

struct Ray {
	vec3 origin; // point at param = Ray.origin + direction * t;
	vec3 direction;
};
typedef struct Ray Ray;

float hit_sphere (vec3 centre, float radius, Ray ray) {
	vec3 oc = ray.origin - centre;
	float a = dot_vec3 (ray.direction, ray.direction);
	float b = 2.0f * dot_vec3 (oc, ray.direction);
	float c = dot_vec3 (oc, oc) - radius * radius;
	float discrim = b * b - 4.0f * a * c;
	if (discrim < 0.0f) {
		return -1.0f;
	} else {
		return (-b - sqrt (discrim)) / 2.0f * a;
	}
}

vec3 colour_ray (Ray ray) {
	vec3 sph_centre = {0.0f, 0.0f, -1.0f};
	float t = hit_sphere (sph_centre, 0.5f, ray);
	if (t > 0.0f) {
		vec3 pap = ray.origin + ray.direction * t;
		vec3 sph_normal = normalise_vec3 (pap - sph_centre);
		vec3 colour = (sph_normal + 1.0f) * 0.5f;
		return colour;
	}
	vec3 udir = normalise_vec3 (ray.direction);
	t = 0.5 * udir[1] + 1.0;
	vec3 op = (vec3){1.0f, 1.0f, 1.0f} * (1.0f - t) +
		(vec3){0.5f, 0.7f, 1.0f} * t;
	return op;
}

int main () {
	int nx = 200, ny = 100;
	printf ("P3\n%i %i\n255\n", nx, ny);
	// viewport/camera size
	vec3 lower_left_corner = {-2.0f, -1.0f, -1.0f}; // of screen
	vec3 horizontal = {4.0f, 0.0f, 0.0f}; // width of screen
	vec3 vertical = {0.0f, 2.0f, 0.0f}; // height of screen
	vec3 origin = {0.0f, 0.0f, 0.0f}; // behind screen
	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			float u = (float)i / (float)nx;
			float v = (float)j / (float)ny;
			
			Ray r;
			r.origin = origin;
			r.direction = lower_left_corner + horizontal * u + vertical * v;
			
			vec3 colour = colour_ray (r);
			
			int ir = (int)(255.99 * colour[0]);
			int ig = (int)(255.99 * colour[1]);
			int ib = (int)(255.99 * colour[2]);
			printf ("%i %i %i\n", ir, ig, ib);
		}
	}

	return 0;
}
