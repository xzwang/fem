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

	fp = fopen(fname, "w+");
	if (fp == NULL)
	{
		return -1;
	}

	while (cnt--)
	{
		fprintf(fp, "%f\n", *data++);
	}

	fclose(fp);

	return 0;
}

#define DEBUG
int frequency_domain_analysis(char *fname, float *dat, int cnt)
{
	float An;
	float Pn;
	FILE *fp = NULL;
	float *rd, *id;
	float K,P;

	if (fname == NULL) {
		return -1;
	}

	if ((fp = fopen(fname, "w+")) == NULL) {
		return -2;
	}

	fprintf(fp, "\t实部\t虚部\t幅值\t相位\n");

	rd = (float *)&dat[0];
	id = (float *)&dat[1];
	P = 180/(4.0 * atan(1.0));
	K = (float)2/cnt;
	while (cnt--) {
		An = sqrtf((*rd) * (*rd) + (*id) * (*id));
		An *= K;
		Pn = atan2f(*id, *rd);
		Pn *= P;
#ifdef DEBUG
		fprintf(fp, "\t%f\t%f\t%f\t%f\n", *rd, *id, An, Pn);
#endif
		rd+=2;
		id+=2;
	}

	fclose(fp);

	return 0;
}


void sinx_gen(void)
{
#define N	1024
	float PI;
	float data[N];
	int i;
	int fs, f0;

	memset(data, 0x0, sizeof(data));

	/* 初始化正弦曲线 */
	PI = 4.0 * atan(1.0);
	fs = 80;
	f0 = 10;
	for (i = 0; i < N; i++) {
		data[i] = sin(2*PI*f0*i/fs);
		fprintf(stderr, "[%d] = %f\n", i, data[i]);
	}

}

#define TXT_NAME	"./64_347.txt"
#define POINT		(1000*24 + 10)
int main(int argc, char *argv[])
{
	float *fft1, *fft2;
	int ret = -1;
	int cnt;
	float *dat1, *dat2;
	float amp, ph;
	struct timeval tv1, tv2;
	int n = 10;

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

	dat2 = (float *)malloc(sizeof(float) * cnt *2);
	if ((dat1 = FFT_INIT(cnt, DFT_1D_R2C)) != NULL)
	{
		gettimeofday(&tv1, NULL);
		while (n--) {
			ret = FFT_DFT(fft1, cnt);
			/* if (ret >= 0) { */
			/* frequency_domain_analysis("fem-dat1.txt", dat1, cnt); */
			/* ret = FFT_DFT_COPY(dat2); */
			/* frequency_domain_analysis("fem-dat2.txt", dat2, cnt); */
			/* } */
		}
		gettimeofday(&tv2, NULL);
		fprintf(stderr, "FFT-DFT :%ldus\n", tv2.tv_sec*1000000+tv2.tv_usec - tv1.tv_sec*1000000-tv1.tv_usec);
		FFT_DFT_AMP_PHA(24000, 64, &amp, &ph);
		fprintf(stderr, "幅值:%f\t相位:%f\n", amp, ph);
	}

	FFT_CLR();
exit2:
	free(fft2);
exit1:
	free(fft1);
exit0:
	return 0;
}
