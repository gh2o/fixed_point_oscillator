#include <stdio.h>

static int clamped_add(int a, int b) {
	int c = (unsigned int)a + (unsigned int)b;
	int asb = a < 0, bsb = b < 0, csb = c < 0;
	if (asb != bsb || asb == csb)
		return c;
	else
		return 0x80000000 - csb;
}

static int clamped_sub(int a, int b) {
	int c = (unsigned int)a - (unsigned int)b;
	int asb = a < 0, bsb = b < 0, csb = c < 0;
	if (asb == bsb || asb == csb)
		return c;
	else
		return 0x80000000 - csb;
}

static int attenuate_delta(int x) {
	x >>= 2;
	return x;
}

static int dt_major(int x) { return (x + x + x) >> 1; }
static int dt_minor(int x) { return x >> 1; }

int main() {
	FILE *mystdout = stdout;
	setbuf(mystdout, NULL);

	int vcos0 = 0x0;
	int vcos  = 0x4000;
	int vsin0 = 0x0;
	int vsin  = 0x0;

	for (int i = 0; i < 4800000; i++) {
		int nvcos = clamped_sub(vcos, dt_major(attenuate_delta(vsin)) - dt_minor(attenuate_delta(vsin0)));
		int nvsin = clamped_add(vsin, dt_major(attenuate_delta(vcos)) - dt_minor(attenuate_delta(vcos0)));
		vcos0 = vcos;
		vsin0 = vsin;
		if (vsin < 0 && nvsin >= 0) {
			vcos = 0x4000;
			vsin = 0x0;
		} else {
			vcos = nvcos;
			vsin = nvsin;
		}
		/*
		fwrite(&vcos, 4, 1, mystdout);
		fwrite(&vsin, 4, 1, mystdout);
		*/

		int c = vcos << 16;
		int s = vsin << 16;
		fwrite(&c, 4, 1, mystdout);
		fwrite(&s, 4, 1, mystdout);
	}
	
	return 0;
}
