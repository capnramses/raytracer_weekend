#include "main.h"
#include "posixthreads.h"
#include <pthread.h>

int tretvals[MAXTHREADS];
pthread_t pthreads[MAXTHREADS];

void* mythread (void* data) {
	assert (data);
	Thread_Data* td = (Thread_Data*)data;
	//fprintf (stderr, "start thread %i,%i\n", td->x, td->y);
	vec3 colour = {0.0f, 0.0f, 0.0f};
	//int id = *(int*)data;
	//fprintf (stderr, "starting thread %i\n", id);
	for (int sample = 0; sample < NSAMPLES; sample++) {
		double u = ((double)td->x + drand48 ()) / (double)NX;
		double v = ((double)td->y + drand48 ()) / (double)NY;
		td->ray.direction = lower_left_corner + horizontal * (float)u + vertical * (float)v;
		colour += colour_ray (td->ray, &world, 0);
	}
	colour /= (float)NSAMPLES;
	// gamma correction
	colour = (vec3){sqrt (colour.x), sqrt (colour.y), sqrt (colour.z)};
	{ // write to memory (NY - j - 1 instead of just j to y-flip)
		int curr = (NY - td->y - 1) * NX * 3 + td->x * 3;
		img_data[curr] = (int)(255.0 * colour.x); // r
		img_data[curr + 1] = (int)(255.0 * colour.y); // g
		img_data[curr + 2] = (int)(255.0 * colour.z); // b
	}
	//fprintf (stderr, "fin thread %i,%i\n", td->x, td->y);
	return 0;
}

void wait_threads (int n) {
	for (int i = 0; i < n; i++) { pthread_join (pthreads[i], NULL); }
}

void create_threads (int n) {
	//printf ("launching %i threads...\n", n);
	for (int i = 0; i < n; i++) {
		tretvals[i] = pthread_create (&pthreads[i], NULL, mythread, &thread_data[i]);
		//assert (tretvals[i]);
	}
	//printf ("launched %i threads. waiting...\n", n);
	wait_threads (n);
	//printf ("threads done;");
}

