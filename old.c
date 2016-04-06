
void write_ppm (unsigned char* data, int x, int y, int c) {
	assert (data);
	FILE* fp = fopen ("img.ppm", "w");
	assert (fp);
	fprintf (fp, "P3\n%i %i\n255\n", x, y);
	for (int yi = y - 1; yi >= 0; yi--) {
		for (int xi = 0; xi < x; xi++) {
			int curr = yi * x * c + xi * c;
			fprintf (fp, "%i %i %i\n", data[curr], data[curr + 1], data[curr + 2]);
		}
	}
	fclose (fp);
}
