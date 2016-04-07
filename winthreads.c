#include "main.h"
#include "winthreads.h"
#include <windows.h>

HANDLE tretvals[MAXTHREADS];

DWORD WINAPI mythread (void* data) {
	assert (data);
	Thread_Data* td = (Thread_Data*)data;
	//fprintf (stderr, "start thread %i,%i\n", td->x, td->y);
	vec3 colour = {0.0f, 0.0f, 0.0f};
	//int id = *(int*)data;
	//fprintf (stderr, "starting thread %i\n", id);
 	for (int sample = 0; sample < NSAMPLES; sample++) {
		double u = ((double)td->x + myrand ()) / (double)NX;
		double v = ((double)td->y + myrand ()) / (double)NY;
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
	//Sleep (2000);
	assert (n <= MAXIMUM_WAIT_OBJECTS);
	WaitForMultipleObjects (n, tretvals, TRUE, INFINITE);
}

void create_threads (int n) {
	//printf ("launching %i threads...\n", n);
	for (int i = 0; i < n; i++) {
		tretvals[i] = CreateThread (NULL, 0, mythread, &thread_data[i], 0, NULL);
		assert (tretvals[i]);
	}
	//printf ("launched %i threads. waiting...\n", n);
	wait_threads (n);
	//printf ("threads done;");
}