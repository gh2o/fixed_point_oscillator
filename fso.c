#include <stdio.h>

static int attenuate_delta(int x, int fac) {
	return (x * fac) >> 12;
}

static int dt_combine(int cur, int prv) {
	return (cur + cur + cur - prv) >> 1;
}

struct osc_state {
	int vcos0, vcos, vsin0, vsin, zcross;
};

static void osc_reset(struct osc_state *os, int init) {
	*os = (struct osc_state){};
	os->vcos = init;
}

static void osc_cycle(struct osc_state *os, int fac, int init) {
	int vcos = os->vcos;
	int vsin = os->vsin;
	int nvcos = vcos - attenuate_delta(dt_combine(vsin, os->vsin0), fac);
	int nvsin = vsin + attenuate_delta(dt_combine(vcos, os->vcos0), fac);
	os->vcos0 = vcos;
	os->vsin0 = vsin;
	int zcrocc = (vsin < 0 && nvsin >= 0);
	if (zcrocc && os->zcross == 63) {
		os->vcos = init;
		os->vsin = 0;
		os->zcross = 0;
	} else {
		os->vcos = nvcos;
		os->vsin = nvsin;
		os->zcross += zcrocc;
	}
}

int main() {
	//FILE *mystdout = stdout;
	//setvbuf(mystdout, NULL, _IOFBF, 48 * 4);

	struct osc_state os;

	for (int j = 4; j <= 4096; j++) {
		osc_reset(&os, 0x40000);

		int pcr;
		while (pcr < 16) {
			osc_cycle(&os, j, 0x40000);
			if (os.zcross == 0)
				pcr++;
		}

		int cyc = 0;
		do {
			osc_cycle(&os, j, 0x40000);
			cyc++;
		} while (os.zcross == 0);
		do {
			osc_cycle(&os, j, 0x40000);
			cyc++;
		} while (os.zcross != 0);

		printf("%d\t%d\t%f\n", j, cyc, 48000.0 * 64 / cyc);
	}
	
	return 0;
}
