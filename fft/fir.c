/***********************************************************
 * Copyright (C), 2013-2014, QiangJun Tech. Co., Ltd.
 * All Rights Reserved.
 * Filename: firfilter.c
 * Author: xz.wang
 * Last modified: 2014-05-08 16:28
 * Description:		version:
 **********************************************************/
#include "fir.h"
#include "fft.h"

static float *float_lh_sinc(int type, int length, int fs, int fc)
{
	int n, half;
	float *sinc;
	float m_2, ft;
	float tmp, wc;
	float PI = (4.0)*atan(1.0);

	if (type == HIGH_PASS && !(length & 0x01)) {

		fprintf(stderr, "For high pass filter, window length must be odd\n");
		return NULL;
	}

	sinc = (float *)malloc(sizeof(float) * length);
	if (!sinc){
		return NULL;
	}

	ft = (float)fc/fs;
	m_2 = 0.5 * (length-1);	//order/2
	half = length/2;

	if (length & 0x01) {	//set center tap
		tmp = 2.0 * ft;
		if (type == HIGH_PASS)
			tmp = 1 - tmp;

		sinc[half] = tmp;
	}

	if (type == HIGH_PASS)
		ft = -ft;

	wc = 2 * ft * PI;
	for (n = 0; n < half; n++) {
		tmp = n - m_2;
		sinc[n] = sinf(wc * tmp)/(PI * tmp);
		sinc[length-1-n] = sinc[n];
	}

	return sinc;
}

static float *float_bd_sinc(int type, int length, int fs, int fc1, int fc2)
{
	int n, half;
	float *sinc;
	float m_2, ft1, ft2;
	float tmp, val1, val2;
	float PI = (4.0)*atan(1.0);

	if (!(length & 0x01)) {

		fprintf(stderr, "For band-pass/band-stop filter, window length must be odd\n");
		return NULL;
	}

	sinc = (float *)malloc(sizeof(float) * length);
	if (!sinc){
		return NULL;
	}

	ft1 = (float)fc1/fs;
	ft2 = (float)fc2/fs;

	m_2 = 0.5 * (length-1);	//order/2
	half = length/2;

	tmp = 2.0 * (ft2 - ft1);
	if (type == BAND_STOP)
		tmp = 1 - tmp;

	sinc[half] = tmp;

	if (type == BAND_STOP)
	{
		tmp = ft2;
		ft2 = ft1;
		ft1 = tmp;
	}

	for (n = 0; n < half; n++) {
		tmp = n - m_2;
		val1 = sinf(2 * PI * ft1 * tmp)/(PI * tmp);
		val2 = sinf(2 * PI * ft2 * tmp)/(PI * tmp);
		sinc[length-1-n] = sinc[n] = val2 - val1;
	}

	return sinc;
}

float zero_bessel(float x)
{
        float num, fact, i;
        float result = 1;
        float x_2 = x/2;

	num = 1;
	fact = 1;
        for (i = 1 ; i < 20 ; i++) {
                num *= x_2 * x_2;
                fact *= i;
                result += num / (fact * fact);
        }

        return result;
}

static int float_create_window(int type, int length, float **coeff)
{
	int n, half;
	int order = length - 1;
	float *window = NULL;
	float PI = (4.0)*atan(1.0);

	window = (float *)malloc(sizeof(float) * length);
	if (!window) {
		return -1;
	}

	half = length/2;

	switch (type) {
		case RECTANGULAR:
			for (n = 0; n <= half; n++) {
				window[n] = 1.0;
				window[length-1-n] = 1.0;
			}
			break;
		case HANNING:
			for (n = 0; n <= half; n++) {
				window[n] = 0.5 - 0.5 * cosf((2 * PI * n)/order);
				window[length-1-n] = window[n];
			}
			break;
		case HAMMING:
			for (n = 0; n <= half; n++) {
				window[n] = 0.54 - 0.46 * cosf((2* PI * n) / order);
				window[length-1-n] = window[n];
			}
			break;
		case BLACKMAN:
			for (n = 0; n <= half; n++) {
				window[n] = 0.42 - 0.5 * cosf(2.0 * PI * n / order) + 0.08 * cos(4.0 * PI * n / order);
				window[length-1-n] = window[n];
			}
			break;
		case BARTLETT:
			for (n = 0; n <= half; n++) {
				float tmp = (float)n - (float)order/2;
				window[n] = 1.0 - 2 * fabs(tmp)/order;
				window[length-1-n] = window[n];
			}
			break;
		default:
			*coeff = NULL;
			free(window);
			fprintf(stderr, "Allocate window buffer failed: Invalid window type!!!\n");
			return -1;
	}

	*coeff = window;

	return 0;
}

int float_fir_filter(float *in, float *out, int length, float *coeff, int coeff_num)
{
	float *buffer, msum;
	int i, n;
	int order = coeff_num - 1;

	buffer = (float *)malloc(sizeof(float) * coeff_num);
	if (!buffer) {
		return -1;
	}

	memset(buffer, 0x0, sizeof(float) * coeff_num);

	/* Support for in-situ operation */
	/* memset(out, 0x0, sizeof(float) * length); */

	buffer[0] = *in++;

	for (n = 0; n < length; n++) {

		msum = 0.0;
		for (i = 0; i < coeff_num; i++)
			msum += buffer[i] * coeff[i];
		out[n] = msum;

		for (i = order; i > 0; i--)
			buffer[i] = buffer[i-1];

		buffer[0] = *in++;
	}

	free(buffer);

	return 0;
}

int calc_kaiser_params(float ripple, float transwidth, float fs, float *beta)
{
        float dw = 2 * M_PI * transwidth / fs;	//delta w
        float a = -20.0 * log10f(ripple);	//ripple dB
        int m;
	int length;

        if (a > 21)
		m = ceil((a-7.95) / (2.285*dw));
        else
		m = ceil(5.79/dw);

        length = m + 1;

        if (a <= 21)
		*beta = 0.0;
        else if (a <= 50)
		*beta = 0.5842 * pow(a-21, 0.4) + 0.07886 * (a-21);
        else
		*beta = 0.1102 * (a-8.7);

	return length;
}

int create_kaiser_window(int length, float beta, float **coeff)
{
        int n;
        float denom = zero_bessel(beta);
	float *window, val;
        float m_2 = 0.5 *(length - 1);	//ORDER/2

	window = (float *) malloc(length * sizeof(float));
	if (!window) {
		fprintf(stderr, "Could not allocate memory for window\n");
		return -1;
	}

        for (n = 0 ; n < length; n++)
        {
                val = ((n) - m_2) / m_2;
                val = 1 - (val * val);
                window[n] = zero_bessel(beta * sqrtf(val)) / denom;
        }

	*coeff = window;

        return 0;
}

float *alloc_kaiser_coeff(int filter_type, int *coeff_len, int fs, int fc1, int fc2)
{
	int n, ret;
	float ripple= 0.001;
	float transwidth = 800;
	float beta;
	float *sinc, *coeff;

	if (filter_type != LOW_PASS) {
		fprintf(stderr, "kaiser window only suported low-pass\n");
		return NULL;
	}

	*coeff_len = calc_kaiser_params(ripple, transwidth, fs, &beta);

	sinc = float_lh_sinc(filter_type, *coeff_len, fs, fc1);
	if (!sinc) {
		return NULL;
	}

	ret = create_kaiser_window(*coeff_len, beta, &coeff);
	if (ret == 0) {
		for (n = 0; n < *coeff_len; n++) {
			coeff[n] *= sinc[n];
			/* fprintf(stderr, "kaiser %d, %f %f\n", n, sinc[n], coeff[n]); */
		}
	}
	else {
		coeff = NULL;
	}

	free(sinc);

	return coeff;
}

float *alloc_filter_coeff(int filter_type, int window_type, int length, int fs, int fc1, int fc2)
{
	float *coeff = NULL;
	float *sinc = NULL;;
	int n, ret;

	switch (filter_type) {
		case LOW_PASS:
		case HIGH_PASS:
			sinc = float_lh_sinc(filter_type, length, fs, fc1);
			break;
		case BAND_PASS:
		case BAND_STOP:
			sinc = float_bd_sinc(filter_type, length, fs, fc1, fc2);
			break;

		default:
			break;
	}
	if (!sinc) {
		return NULL;
	}

	ret = float_create_window(window_type, length, &coeff);
	if (ret == 0) {
		for (n = 0; n < length; n++) {
			coeff[n] *= sinc[n];
			/* fprintf(stderr, "%d, %f %f\n", n, sinc[n], coeff[n]); */
		}
	}
	else {
		coeff = NULL;
	}

	free(sinc);

	return coeff;
}

void free_filter_coeff(float *coeff)
{
	if (!coeff)
		free(coeff);
}

static int plot_filter(char *fname, float *dat, int fs, int cnt)
{
	FILE *fp = NULL;
	int i;
	float *rl, *ig;
	float An, dB, Pn;
	float freq;

	if ((fp = fopen(fname, "w+")) == NULL) {
		return -1;
	}

	fprintf(fp, "#num\tfreq\tmag\tmagdB\tphase\n");

	rl = (float *)&dat[0];
	ig = (float *)&dat[1];
	/* P = 180/(4.0 * atan(1.0)); */
	/* K = (float)2/cnt; */

	for (i = 0; i < cnt/2; i++)
	{
		freq = (float)fs * i / cnt;
		An = sqrtf((*rl) * (*rl) + (*ig) * (*ig));
		/* An *= K; */
		dB = 20 * log10f(An);
		Pn = atan2f(*ig, *rl);
		/* Pn *= P; */ //rad

		fprintf(fp, "%d %f %f %f %f\n", i, freq, An, dB, Pn);
		rl+=2;
		ig+=2;
	}

	fclose(fp);

	return 0;
}

void fir_out(char *fname, float *coeff, int coeff_num, int fs)
{
	float *out;
	int length = 2048;

	out = (float *)malloc(sizeof(float) * 2 * length);
	if (FFT_INIT(length, DFT_1D_R2C) != NULL) {
		FFT_DFT(coeff, coeff_num);
		FFT_DFT_COPY(out);
		FFT_CLR();
	}
	plot_filter(fname, out, fs, length);

	free(out);
}

int fir_test(void)
{
	int window_len = 31;
	int fs = 24000;
	int fc1 = 6000;
	int fc2 = 8000;
	float *coeff;

	// LOW-PASS
	coeff = alloc_filter_coeff(LOW_PASS, HAMMING, window_len, fs, fc1, fc2);
	if (coeff != NULL) {
		fir_out("lp_hamming.dat", coeff, window_len, fs);
		free_filter_coeff(coeff);
	}

	coeff = alloc_filter_coeff(LOW_PASS, HANNING, window_len, fs, fc1, fc2);
	if (coeff != NULL) {
		fir_out("lp_hanning.dat", coeff, window_len, fs);
		free_filter_coeff(coeff);
	}

	coeff = alloc_filter_coeff(LOW_PASS, BLACKMAN, window_len, fs, fc1, fc2);
	if (coeff != NULL) {
		fir_out("lp_blkman.dat", coeff, window_len, fs);
		free_filter_coeff(coeff);
	}

	int kaiser_len;
	coeff = alloc_kaiser_coeff(LOW_PASS, &kaiser_len, fs, fc1, fc2);
	if (coeff != NULL) {
		fir_out("lp_kaiser.dat", coeff, kaiser_len, fs);
		free_filter_coeff(coeff);
	}

	//High-PASS
	coeff = alloc_filter_coeff(HIGH_PASS, HAMMING, window_len, fs, fc1, fc2);
	if (coeff != NULL) {
		fir_out("hp_hamming.dat", coeff, window_len, fs);
		free_filter_coeff(coeff);
	}

	coeff = alloc_filter_coeff(HIGH_PASS, HANNING, window_len, fs, fc1, fc2);
	if (coeff != NULL) {
		fir_out("hp_hanning.dat", coeff, window_len, fs);
		free_filter_coeff(coeff);
	}

	coeff = alloc_filter_coeff(HIGH_PASS, BLACKMAN, window_len, fs, fc1, fc2);
	if (coeff != NULL) {
		fir_out("hp_blkman.dat", coeff, window_len, fs);
		free_filter_coeff(coeff);
	}

	//BAND_PASS
	fc1 = 3000;
	fc2 = 9000;
	coeff = alloc_filter_coeff(BAND_PASS, HAMMING, window_len, fs, fc1, fc2);
	if (coeff != NULL) {
		fir_out("bp_hamming.dat", coeff, window_len, fs);
		free_filter_coeff(coeff);
	}

	coeff = alloc_filter_coeff(BAND_PASS, HANNING, window_len, fs, fc1, fc2);
	if (coeff != NULL) {
		fir_out("bp_hanning.dat", coeff, window_len, fs);
		free_filter_coeff(coeff);
	}

	coeff = alloc_filter_coeff(BAND_PASS, BLACKMAN, window_len, fs, fc1, fc2);
	if (coeff != NULL) {
		fir_out("bp_blkman.dat", coeff, window_len, fs);
		free_filter_coeff(coeff);
	}

	return 0;
}
