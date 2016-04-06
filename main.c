// up to Chapter 10: Pos Cam

#include "apg_maths_clang.h"
#define __USE_SVID // drand48
#include <stdlib.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
#include <assert.h>

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
inline vec3 reflect (vec3 v, vec3 n);
inline vec3 refract (vec3 v, vec3 n, float ni_over_nt, bool* refracted);
vec3 colour_ray (Ray ray, const World* world, int depth);

inline vec3 rand_in_unit_sphere ();
vec3 rand_in_unit_sphere () {
	vec3 p;
	do {
		p = 2.0f * (vec3){drand48 (), drand48 (), drand48 ()} - (vec3){1.0f, 1.0f, 1.0f};
	} while (dot_vec3 (p, p) >= 1.0f);
	return p;
}

vec3 reflect (vec3 v, vec3 n) { return v - 2.0f * dot_vec3 (v, n) * n; }

vec3 refract (vec3 v, vec3 n, float ni_over_nt, bool* refracted) {
	assert (refracted);
	*refracted = false;
	vec3 uv = normalise_vec3 (v);
	float dt = dot_vec3 (uv, n);
	float discrim = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
	if (discrim > 0.0f) {
		*refracted = true;
		float sqd = sqrt (discrim);
		return ni_over_nt * (v - n * dt) - n * sqd;
	}
	return v;
}

// fresnel approx -- at steep angles reflect approaches perfect mirror
float schlick (float cosine, float ref_idx) {
	float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * powf ((1.0f - cosine), 5.0f);
}

Ray scatter_lambert (Ray inray, Hit_Record hr) {
	vec3 target = hr.p + hr.n + rand_in_unit_sphere ();
	Ray outray;
	outray.origin = hr.p;
	outray.direction = target - hr.p;
	outray.tmin = inray.tmin;
	outray.tmax = inray.tmax;
	return outray;
}

// atten == albedo for metal
Ray scatter_metal (Ray inray, Hit_Record hr, bool* bounced) {
	assert (bounced);
	vec3 reflray = reflect (normalise_vec3 (inray.direction), hr.n);
	Ray outray;
	outray.origin = hr.p;
	// reflection + blur in unit sphere
	outray.direction = reflray + hr.material.fuzz * rand_in_unit_sphere ();
	outray.tmin = inray.tmin;
	outray.tmax = inray.tmax;
	*bounced = false;
	if (dot_vec3 (reflray, hr.n) > 0.0f) { *bounced = true; }
	return outray;
}

Ray scatter_dielectric (Ray inray, Hit_Record hr) {
	Ray outray;
	outray.tmin = inray.tmin;
	outray.tmax = inray.tmax;
	vec3 outward_normal;
	vec3 reflected_dir = reflect (inray.direction, hr.n);
	float ni_over_nt = 1.0f, reflect_prob = 1.0f, cosine = 0.0f;
	if (dot_vec3 (inray.direction, hr.n) > 0.0f) {
		outward_normal = -hr.n;
		ni_over_nt = hr.material.ior;
		cosine = hr.material.ior * dot_vec3 (inray.direction, hr.n) /
			length_vec3 (inray.direction);
	} else {
		outward_normal = hr.n;
		ni_over_nt = 1.0f / hr.material.ior;
		cosine = -dot_vec3 (inray.direction, hr.n) / length_vec3 (inray.direction);
	}
	bool refracted = false;
	vec3 refracted_dir = refract (inray.direction, outward_normal, ni_over_nt,
		&refracted);
	if (refracted) {
		reflect_prob = schlick (cosine, hr.material.ior);
	}
	if (drand48 () < reflect_prob) {
		outray.origin = hr.p;
		outray.direction = reflected_dir;
	} else {
		outray.origin = hr.p;
		outray.direction = refracted_dir;
	}
	return outray;
}

bool hit_spheres (Ray ray, const Sphere* spheres, Hit_Record* hr) {
	assert (spheres); assert (hr);
	bool hit_anything = false;
	for (int i = 0; i < NSPHERES; i++) {
		vec3 oc = ray.origin - spheres[i].centre;
		float a = dot_vec3 (ray.direction, ray.direction);
		float b = dot_vec3 (oc, ray.direction);
		float c = dot_vec3 (oc, oc) - spheres[i].radius * spheres[i].radius;
		float discrim = b * b - a * c;
		if (discrim > 0.0f) {
			float srd = sqrt (discrim);
			float tmp = (-b - srd) / a;
			if (tmp < ray.tmax && tmp > ray.tmin && tmp < hr->t) {
				hr->t = tmp;
				hr->p = ray.origin + ray.direction * tmp; // point_at_param()
				hr->n = normalise_vec3 ((hr->p - spheres[i].centre) / spheres[i].radius);
				memcpy (&hr->material, &spheres[i].material, sizeof (Material));
				hit_anything = true;
			}
			tmp = (-b + srd) / a;
			if (tmp < ray.tmax && tmp > ray.tmin && tmp < hr->t) {
				hr->t = tmp;
				hr->p = ray.origin + ray.direction * tmp;
				hr->n = normalise_vec3 ((hr->p - spheres[i].centre) / spheres[i].radius);
				memcpy (&hr->material, &spheres[i].material, sizeof (Material));
				hit_anything = true;
			}
		}//endif
	}//endfor
	return hit_anything;
}

vec3 colour_ray (Ray ray, const World* world, int depth) {
	Hit_Record hr;
	hr.t = FLT_MAX;
	hr.material.type = 0;
	hr.material.albedo = (vec3){0.0f, 0.0f, 0.0f};
	hr.material.fuzz = 0.0f;
	hr.material.ior = 1.0f;
	bool hit_anything = false;
	if (hit_spheres (ray, world->spheres, &hr)) { hit_anything = true; }
	if (hit_anything) {
		Ray scattray;
		bool valid = true, bounced = true;
		vec3 atten;
		switch (hr.material.type) {
			case 0:
				scattray = scatter_lambert (ray, hr);
				atten = hr.material.albedo;
			break;
			case 1:
				scattray = scatter_metal (ray, hr, &bounced);
				atten = hr.material.albedo;
				if (!bounced) { valid = false; }
			break;
			case 2:
				scattray = scatter_dielectric (ray, hr);
				atten = (vec3){1.0f, 1.0f, 1.0f};
			break;
			default: ;
		}
		if (depth < 50 && valid) {
			return atten * colour_ray (scattray, world, depth + 1);
		} else {
			return (vec3){0.0f, 0.0f, 0.0f};
		}
	}
	return (vec3){0.5f, 0.7f, 1.0f};
}

void define_world (World* world) {
	assert (world);
	world->spheres[0].centre = (vec3){0.0f, 0.0f, -1.0f};
	world->spheres[0].radius = 0.5f;
	world->spheres[0].material.type = 0;
	world->spheres[0].material.albedo = (vec3){0.8f, 0.3f, 0.3f};
	world->spheres[0].material.fuzz = 0.0f;
	world->spheres[1].centre = (vec3){0.0f, -100.5f, -1.0f};
	world->spheres[1].radius = 100.0f;
	world->spheres[1].material.type = 0;
	world->spheres[1].material.albedo = (vec3){0.8f, 0.8f, 0.0f};
	world->spheres[1].material.fuzz = 0.0f;
	world->spheres[2].centre = (vec3){1.0f, 0.0f, -1.0f};
	world->spheres[2].radius = 0.5f;
	world->spheres[2].material.type = 1;
	world->spheres[2].material.albedo = (vec3){0.8f, 0.6f, 0.2f};
	world->spheres[2].material.fuzz = 0.3f;
	world->spheres[3].centre = (vec3){-1.0f, 0.0f, -1.0f};
	// negative radius means normal points inwards == hollow glass sphere
	world->spheres[3].radius = 0.5f;
	world->spheres[3].material.type = 2;
	world->spheres[3].material.albedo = (vec3){0.8f, 0.8f, 0.8f};
	world->spheres[3].material.ior = 1.5f;
}

int main () {
	int nx = 200, ny = 100, nsamples = 100;
	float aspect = (float)nx / (float)ny;
	vec3 lower_left_corner = {-aspect, -1.0f, -1.0f}; // of screen
	vec3 horizontal = {aspect * 2.0f, 0.0f, 0.0f}; // width of screen
	vec3 vertical = {0.0f, 2.0f, 0.0f}; // height of screen
	unsigned char* img_data = (unsigned char*)malloc (nx * ny * 3);
	World world;
	define_world (&world);
	Ray ray;
	ray.origin = (vec3){0.0f, 0.0f, 0.0f}; // behind screen
	ray.tmin = 0.0f;
	ray.tmax = FLT_MAX;
	for (int j = 0; j < ny; j++) {
		for (int i = 0; i < nx; i++) {
			vec3 colour = {0.0f, 0.0f, 0.0f};
			for (int sample = 0; sample < nsamples; sample++) {
				double u = ((double)i + drand48 ()) / (double)nx;
				double v = ((double)j + drand48 ()) / (double)ny;
				ray.direction = lower_left_corner + horizontal * (float)u + vertical * (float)v;
				colour += colour_ray (ray, &world, 0);
			}
			colour /= (float)nsamples;
			// gamma correction
			colour = (vec3){sqrt (colour.x), sqrt (colour.y), sqrt (colour.z)};
			{ // write to memory (ny - j - 1 instead of just j to y-flip)
				int curr = (ny - j - 1) * nx * 3 + i * 3;
				img_data[curr] = (int)(255.99 * colour.x); // r
				img_data[curr + 1] = (int)(255.99 * colour.y); // g
				img_data[curr + 2] = (int)(255.99 * colour.z); // b
			}
		}
	}
	{ // write image and pointless clean up
		//write_ppm (img_data, nx, ny, 3);
		int r = stbi_write_png ("out.png", nx, ny, 3, img_data, 3 * nx); assert (r);
		assert (img_data); free (img_data); img_data = NULL;
	}
	return 0;
}
