#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

#define NUM_TESTS 100000

int do_x(int a, int b, int c);
int do_y(int a, int b, int c);
int do_z(int a, int b, int c);

int s, k, d, f; /* 4 intermediates */

int main(int argc, char** argv) {
	srand(time(NULL));
	int inp[3] = {0};
	long x = 0, y = 0, z = 0;
	int ctr = 0;
	while (ctr++ < NUM_TESTS) {
		for (int i = 0; i < 3; ++i) inp[i] = rand() % 6 + 1;

		int a, b, c;

		/* quickly sort using min/max */
		int low = min(min(inp[0], inp[1]), inp[2]);
		int high = max(max(inp[0], inp[1]), inp[2]);

		b = inp[0] + inp[1] + inp[2] - low - high;
		a = low;
		c = high;

		s = b+c;
		k = (a+5)/7;
		d = (4+c-a)/5+((b-a)*(c-b)+5)/6;
		f = 1^((d/2)*(k^1)*((b^7)/3));

		x += do_x(a, b, c);
		y += do_y(a, b, c);
		z += do_z(a, b, c);
	}

	double x_avg = (double) x / (double) NUM_TESTS;
	double y_avg = (double) y / (double) NUM_TESTS;
	double z_avg = (double) z / (double) NUM_TESTS;

	printf("averages for %u tests: \n\tx = %f\n\ty = %f\n\tz = %f\n", NUM_TESTS, x_avg, y_avg, z_avg);

	return 0;
}

int do_x(int a, int b, int c) {
	return f-(d&1)*((((a-1)%4)+6)/7);
}

int do_y(int a, int b, int c) {
	return f-(d/2)*k*(c/5);
}

int do_z(int a, int b, int c) {
	return 1+(d/2)*(1+k*((15-(a+s))%5)+(k^1)*(s%5))+k*(d&1)*(1+(a+c)%5);
}
