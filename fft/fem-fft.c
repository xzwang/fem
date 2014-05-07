#include "fft.h"
#include <sys/time.h>

int read_raw_txt(char *fname, float *fft1, float *fft2)
{
	int cnt = 0;
	FILE *fp = NULL;

	fp = fopen(fname, "r");
	if (fp == NULL) {
		fprintf(stderr, "Read %s failed\n", fname);
		return -1;
	}

	while (!feof(fp))
	{
		fscanf(fp, "%f %f\n", fft1++, fft2++);
		cnt++;
	}

	fclose(fp);

	return cnt;
}

int write_raw_txt(char *fname, float *data, int cnt)
{
	FILE *fp;
	int i = 0;
	fp = fopen(fname, "w+");
	if (fp == NULL)
	{
		return -1;
	}

	while (cnt--)
	{
		fprintf(fp, "%d %f\n", i++, *data++);
	}

	fclose(fp);

	return 0;
}

#define DEBUG
int fftw_data_plot(char *fname, float *dat, int cnt)
{
	FILE *fp = NULL;
	int i = 0;
	float *rl, *ig;
	float K,An, dB, Pn, P;

	if ((fp = fopen(fname, "w+")) == NULL) {
		return -1;
	}

	fprintf(fp, "#num\t幅度\t幅度\t相频\n");

	rl = (float *)&dat[0];
	ig = (float *)&dat[1];
	P = 180/(4.0 * atan(1.0));
	K = (float)2/cnt;
	while (cnt--)
	{
		An = sqrtf((*rl) * (*rl) + (*ig) * (*ig));
		An *= K;
		dB = 20 * log10f(An);
		Pn = atan2f(*ig, *rl);
		/* Pn *= P; */ //rad

		fprintf(fp, "%d %f %f %f\n", i++, An, dB, Pn);
		rl+=2;
		ig+=2;
	}

	fclose(fp);

	return 0;
}

float *sinx_gen(int cnt, float amp, float p, int fs, int f0)
{
#define N	1024
	float PI;
	int i;
	float *dat;


	dat = (float *)malloc(sizeof(float) * cnt);
	if (dat == NULL)
	{
		return NULL;
	}

	memset(dat, 0x0, sizeof(float) * cnt);

	/* 初始化正弦曲线 */
	PI = 4.0 * atan(1.0);
	for (i = 0; i < cnt; i++) {
		dat[i] = amp * sin(2*PI*f0*i/fs + p);
		/* dat[i] = amp*sin(2*PI*0.25*i)/(PI*i); */
	}

	return dat;
}

void sinx_free(float *sinx)
{
	if (!sinx)
		free(sinx);
}

#define TXT_NAME	"./64_347.txt"
#define POINT		(1000*24 + 10)

#if 0
int main(void)
{
	float *sinx, *dat1, *dat2;
	int cnt =1024;
	int fs = 1024;
	int f0 = 64;
	float amp, ph;
	int ret;

	sinx = sinx_gen(cnt, 12.75, 1.7, fs, f0);
	if (sinx == NULL)
	{
		exit(0);
	}

	write_raw_txt("sinx.txt", sinx, cnt);
	dat2 = (float *)malloc(sizeof(float)*2*cnt);

	if ((dat1 = FFT_INIT(cnt, DFT_1D_C2C)) != NULL)
	{
		ret = FFT_DFT(sinx, cnt);
		if (ret >= 0) {
			FFT_DFT_COPY(dat2);
			fftw_data_plot("Sinx.dat", dat2, cnt);
		}
		/* if (ret >= 0) { */
		/* frequency_domain_analysis("fem-dat1.txt", dat1, cnt); */
		/* ret = FFT_DFT_COPY(dat2); */
		/* frequency_domain_analysis("fem-dat2.txt", dat2, cnt); */
		/* } */
		FFT_DFT_AMP_PHA(fs, f0, &amp, &ph);
		fprintf(stderr, "幅值:%f\t相位:%f\n", amp, ph);
	}
	free(dat2);
	sinx_free(sinx);
	FFT_CLR();

	return 0;
}
#else
int main(int argc, char *argv[])
{
	float *fft1, *fft2;
	int ret = -1;
	int cnt;
	float *dat1, *dat2;
	struct timeval tv1, tv2;
	int n = 1;
	struct fft_t fft;

	if (argc < 2) {
		fprintf(stderr, "fft <filename.txt>\n");
		exit(0);
	}

	fft1 = malloc(sizeof(float) * POINT);
	if (fft1 == NULL)
	{
		fprintf(stderr, "fft1 memory alloc failed\n");
		goto exit0;
	}

	fft2 = malloc(sizeof(float) * POINT);
	if (fft2 == NULL)
	{
		fprintf(stderr, "fft2 memory alloc failed\n");
		goto exit1;
	}

	cnt = read_raw_txt(argv[1], fft1, fft2);
	if (cnt < 0) {
		goto exit2;
	}

	write_raw_txt("raw1.dat", fft1, cnt);
	write_raw_txt("raw2.dat", fft2, cnt);
	dat2 = (float *)malloc(sizeof(float) * cnt *2);
	if ((dat1 = FFT_INIT(cnt, DFT_1D_C2C)) != NULL)
	{
		/* gettimeofday(&tv1, NULL); */
		/* while (n--) { */
			ret = FFT_DFT(fft1, cnt);
			if (ret >= 0) {
				FFT_DFT_COPY(dat2);
				fftw_data_plot("res1.dat", dat2, cnt);
				FFT_DFT_AMP_PHA(24000, 64, &fft);
				fprintf(stderr, "FFT 幅值:%f\t相位:%f\n", fft.mag, fft.phase);
			}
			ret = FFT_DFT(fft2, cnt);
			if (ret >= 0) {
				FFT_DFT_COPY(dat2);
				fftw_data_plot("res2.dat", dat2, cnt);
				FFT_DFT_AMP_PHA(24000, 64, &fft);
				fprintf(stderr, "FFT 幅值:%f\t相位:%f\n", fft.mag, fft.phase);
			}
		/* } */
		/* gettimeofday(&tv2, NULL); */
		/* fprintf(stderr, "FFT-DFT :%ldus\n", tv2.tv_sec*1000000+tv2.tv_usec - tv1.tv_sec*1000000-tv1.tv_usec); */
		// fast algorithm
		FAST_GOERZTEL_DFT(fft1, cnt, 64, 24000, &fft);
		fprintf(stderr, "GOERZTEL:%f %f\n", fft.mag, fft.phase);

		FAST_GOERZTEL_DFT(fft2, cnt, 64, 24000, &fft);
		fprintf(stderr, "GOERZTEL:%f %f\n", fft.mag, fft.phase);
	}
	free(dat2);

	FFT_CLR();
exit2:
	free(fft2);
exit1:
	free(fft1);
exit0:
	return 0;
}
#endif
