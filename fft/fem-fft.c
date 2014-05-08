#include <sys/time.h>
#include "fft.h"
#include "firfilter.h"

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

int sin_test(void)
{
	float *sinx, *dat1, *dat2;
	int cnt =1024;
	int fs = 1024;
	int f0 = 64;
	float amp = 12.5, ph = 1.7;
	int ret;
	struct fft_t fft;

	sinx = sinx_gen(cnt, amp, ph, fs, f0);
	if (sinx == NULL)
	{
		return 0;
	}

	write_raw_txt("sin_raw.dat", sinx, cnt);
	dat2 = (float *)malloc(sizeof(float)*2*cnt);

	if ((dat1 = FFT_INIT(cnt, DFT_1D_C2C)) != NULL)
	{
		ret = FFT_DFT(sinx, cnt);
		if (ret >= 0) {
			FFT_DFT_COPY(dat2);
			fftw_data_plot("Sinx.dat", dat2, fs, cnt);
		}
		FFT_DFT_AMP_PHA(fs, f0, &fft);
		fprintf(stderr, "幅值:%f\t相位:%f\n", fft.mag, fft.phase);

		FFT_CLR();
	}
	free(dat2);

	sinx_free(sinx);
	return 0;
}

int fem_raw_test(void)
{
	#define TXT_NAME	"./64_347.txt"
	#define POINT		(1000*24 + 10)

	float *raw1, *raw2;
	int cnt, ret;
	float *out;
	int fs = 24000, f0 = 64;
	float *coeff;

	raw1 = malloc(sizeof(float) * POINT);
	if (raw1 == NULL)
	{
		fprintf(stderr, "raw data 1 memory alloc failed\n");
		goto exit0;
	}

	raw2 = malloc(sizeof(float) * POINT);
	if (raw2 == NULL)
	{
		fprintf(stderr, "raw data 2 memory alloc failed\n");
		goto exit1;
	}

	cnt = read_raw_txt(TXT_NAME, raw1, raw2);
	if (cnt < 0) {
		goto exit2;
	}

	write_raw_txt("raw1.dat", raw1, cnt);
	write_raw_txt("raw2.dat", raw2, cnt);


	// Goerztel algorithm test
	FAST_GOERZTEL_DFT(raw1, cnt, f0, fs, &fft);
	fprintf(stderr, "raw1  GOERZTEL:%f %f\n", fft.mag, fft.phase);

	FAST_GOERZTEL_DFT(raw2, cnt, f0, fs, &fft);
	fprintf(stderr, "raw2 GOERZTEL:%f %f\n", fft.mag, fft.phase);

	// FFTW test
	out = (float *)malloc(sizeof(float) * cnt * 2);
	if ((FFT_INIT(cnt, DFT_1D_C2C)) != NULL)
	{
		ret = FFT_DFT(raw1, cnt);
		if (ret >= 0) {
			FFT_DFT_COPY(out);
			fftw_data_plot("out1.dat", out, fs, cnt);
			FFT_DFT_AMP_PHA(fs, f0, &fft);
			fprintf(stderr, "raw1 幅值:%f\t相位:%f\n", fft.mag, fft.phase);
		}

		ret = FFT_DFT(raw2, cnt);
		if (ret >= 0) {
			FFT_DFT_COPY(out);
			fftw_data_plot("res2.dat", out, fs, cnt);
			FFT_DFT_AMP_PHA(fs, f0, &fft);
			fprintf(stderr, "raw2 幅值:%f\t相位:%f\n", fft.mag, fft.phase);
		}

		FFT_CLR();
	}

	//pre-filtel(LOW_PASS) FFTW test
	int fc1 = 1000; //the cutoff frequency
	int fc2 = 5000;
	int coeff_len = 31;
	coeff = alloc_filter_coeff(LOW_PASS, HAMMING, coeff_len, fs, fc1, fc2);
	if (!coeff) {
		if (float_fir_filter(raw1, raw1, cnt, coeff, coeff_len) > 0) {
			if ( FFT_INT(cnt, DFT_1D_C2C) != NULL) {

				FFT_DFT(raw1, cnt);
				FFT_DFT_COPY(out);
				fftw_data_plot("fir-out1.dat", out, fs, cnt);
				FFT_DFT_AMP_PHA(fs, f0, &fft);
				fprintf(stderr, "raw1 幅值:%f\t相位:%f\n", fft.mag, fft.phase);
			}

		}

		free_filler_coeff(coeff);
	}


	free(out);

exit2:
	free(raw1);
exit1:
	free(raw2);
exit0:
	return 0;
}

int main(int argc, char *argv[])
{

}
#endif
