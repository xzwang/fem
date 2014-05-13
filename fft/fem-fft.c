#include <sys/time.h>
#include "fft.h"
#include "fir.h"

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
float *sinwn(int *f0, float *a, float *ph, int num, int cnt, int fs)
{
	float PI = 4.0 * atan(1.0);
	float *dat;
	int i, n;
	float f[4];

	dat = (float *)malloc(sizeof(float) * cnt);
	if (dat == NULL)
	{
		return NULL;
	}

	memset(dat, 0x0, sizeof(float) * cnt);

	for (i = 0; i < num; i++) {
		f[i] = 2.0 * PI * f0[i]/fs;

	}

	for (n = 0; n < cnt; n++) {
		dat[n] = 0;
		for (i = 0; i < num; i++)
			dat[n] = dat[n] + a[i]*sinf(n * f[i] + ph[i]);
	}

	return dat;
}

float *coswn(int *f0, float *a, float *ph, int num, int cnt, int fs)
{
	float PI = 4.0 * atan(1.0);
	float *dat;
	int i, n;

	dat = (float *)malloc(sizeof(float) * cnt);
	if (dat == NULL)
	{
		return NULL;
	}

	memset(dat, 0x0, sizeof(float) * cnt);

	for (n = 0; n < cnt; n++) {
		dat[n] = 2;
		dat[n] = dat[n] + 3*cosf(2*PI*50*n/fs - 0.523) +
			1.5*cosf(2*PI*75*n/fs + 1.570);
	}

	return dat;
}
float *sinx_gen(int cnt, float amp, float p, int fs, int f0)
{
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
		dat[i] = amp * sin(2*PI*f0*i/fs + p * PI/180);
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
	int cnt = 256;
	int fs = 256;
	int f0[3] = {0, 64, 32};
	float amp[3] = {2, 3.0, 1.5}, ph[3] = {0, 0, 0};
	int ret;
	struct fft_t fft;

	sinx = sinwn(f0, amp, ph, 3, cnt, fs);
	/* sinx = coswn(f0, amp, ph, 3, cnt, fs); */
	if (sinx == NULL)
	{
		return 0;
	}

	write_raw_txt("sin_raw.dat", sinx, cnt);

	dat2 = (float *)malloc(sizeof(float) * 2 * cnt);
	if ((dat1 = FFT_INIT(cnt, DFT_1D_R2C)) != NULL)
	{
		ret = FFT_DFT(sinx, cnt);
		if (ret >= 0) {
			FFT_DFT_COPY(dat2);
			fftw_data_plot("sin.dat", dat2, fs, cnt);
		}
		FFT_DFT_AMP_PHA(fs, f0[0], &fft);
		fprintf(stderr, "Sin : %f %f\t", fft.mag, fft.phase);
		FFT_DFT_AMP_PHA(fs, f0[1], &fft);
		fprintf(stderr, "%f %f\t", fft.mag, fft.phase);
		FFT_DFT_AMP_PHA(fs, f0[2], &fft);
		fprintf(stderr, "%f %f\n", fft.mag, fft.phase);

		FFT_CLR();
	}
	free(dat2);

	sinx_free(sinx);
	return 0;
}

int fem_raw_test(char *name, int f0)
{
	#define POINT		(1000*24 + 10)

	float *raw1, *raw2;
	int cnt, ret;
	float *out;
	int fs = 24000;
	float *coeff;
	struct fft_t fft;

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

	cnt = read_raw_txt(name, raw1, raw2);
	if (cnt < 0) {
		goto exit2;
	}

	write_raw_txt("raw1.dat", raw1, cnt);
	write_raw_txt("raw2.dat", raw2, cnt);

	// Goerztel algorithm test
	int l = cnt, rand;
	float *tmp = raw1;
	float_goerztel_init(cnt, f0, fs);
	while (l > 10) {
		rand = random()	% l;
		/* fprintf(stderr, "rand(%d) = %d\n", l, rand); */
		float_goerztel_update(tmp, rand);
		l -= rand;
		tmp += rand;
	}
	float_goerztel_final(tmp, l, &fft);
	fprintf(stderr, "1-GZL:%f %f\t", fft.mag, fft.phase);
	FAST_GOERZTEL_DFT(raw1, cnt, f0, fs, &fft);
	fprintf(stderr, "%f %f\n", fft.mag, fft.phase);
	FAST_GOERZTEL_DFT(raw2, cnt, f0, fs, &fft);
	fprintf(stderr, "2-GZL:%f %f\n", fft.mag, fft.phase);

	// FFTW test
	out = (float *)malloc(sizeof(float) * cnt * 2);
	if ((FFT_INIT(cnt, DFT_1D_R2C)) != NULL)
	{
		ret = FFT_DFT(raw1, cnt);
		if (ret >= 0) {
			FFT_DFT_COPY(out);
			fftw_data_plot("out1.dat", out, fs, cnt);
			FFT_DFT_AMP_PHA(fs, f0, &fft);
			fprintf(stderr, "1-FFTW: %f %f\n", fft.mag, fft.phase);
		}

		ret = FFT_DFT(raw2, cnt);
		if (ret >= 0) {
			FFT_DFT_COPY(out);
			fftw_data_plot("out2.dat", out, fs, cnt);
			FFT_DFT_AMP_PHA(fs, f0, &fft);
			fprintf(stderr, "2-FFTW: %f %f\n", fft.mag, fft.phase);
		}

		FFT_CLR();
	}

	//pre-filtel(LOW_PASS) FFTW test
	int fc1 = 5000; //the cutoff frequency
	int fc2 = 0;
	int coeff_len = 31;
	coeff = alloc_filter_coeff(LOW_PASS, HAMMING, coeff_len, fs, fc1, fc2);
	if (coeff != NULL) {
		write_raw_txt("coeff.txt", coeff, coeff_len);
		if (float_fir_filter(raw1, raw1, cnt, coeff, coeff_len) == 0) {
			write_raw_txt("raw11.dat", raw1, cnt);
			if ( FFT_INIT(cnt, DFT_1D_R2C) != NULL) {

				FFT_DFT(raw1, cnt);
				FFT_DFT_COPY(out);
				fftw_data_plot("fir-out1.dat", out, fs, cnt);
				FFT_DFT_AMP_PHA(fs, f0, &fft);
				fprintf(stderr, "1-FIR-fft: %f-%f\n", fft.mag, fft.phase);
			}

		}
		if (float_fir_filter(raw2, raw2, cnt, coeff, coeff_len) == 0) {
			write_raw_txt("raw11.dat", raw2, cnt);
			if ( FFT_INIT(cnt, DFT_1D_R2C) != NULL) {

				FFT_DFT(raw2, cnt);
				FFT_DFT_COPY(out);
				fftw_data_plot("fir-out1.dat", out, fs, cnt);
				FFT_DFT_AMP_PHA(fs, f0, &fft);
				fprintf(stderr, "2-FIR-fft: %f-%f\n", fft.mag, fft.phase);
			}

		}

		free_filter_coeff(coeff);
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

	sin_test();

	fir_test();
	fprintf(stderr, ">>>>>>>>>>>>>f0 = 64Hz\n");
	fem_raw_test("64_347.txt", 64);
	fprintf(stderr, ">>>>>>>>>>>>>f0 = 4096Hz\n");
	fem_raw_test("C409623_347.txt", 4096);
	fprintf(stderr, ">>>>>>>>>>>>>f0 = 8192Hz\n");
	fem_raw_test("C819201_347.txt", 8192);
	fprintf(stderr, ">>>>>>>>>>>>>f0 = 8192Hz\n");
	fem_raw_test("8192_347.txt", 8192);

	return 0;
}
