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
	float *buffer;
	int i, n;
	int order = coeff_num - 1;

	buffer = (float *)malloc(sizeof(float) * coeff_num);
	if (!buffer) {
		return -1;
	}

	memset(buffer, 0x0, sizeof(float) * coeff_num);
	/* memset(out, 0x0, sizeof(float) * length); */

	buffer[0] = *in++;

	for (n = 0; n < length; n++) {

		for (i = 0; i < coeff_num; i++)
			out[n] += buffer[i] * coeff[i];

		for (i = order; i > 0; i--)
			buffer[i] = buffer[i-1];

		buffer[0] = *in++;
	}

	free(buffer);

	return 0;
}

float *alloc_filter_coeff(int filter_type, int window_type, int length, int fs, int fc1, int fc2)
{
	float *coeff = NULL;
	float *sinc = NULL;;
	int n, ret;

	if (fc1 > fc2) {
		return NULL;
	}

	switch(filter_type) {
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
		}
	}

	free(sinc);

	return coeff;
}

void free_filter_coeff(float *coeff)
{
	if (!coeff)
		free(coeff);
}

int fir_test(void)
{
	int window_len = 31;
	int length = 2048;
	int fs = 24000;
	int fc1 = 5000;
	int fc2 = 15000;
	float *coeff, *out;


	coeff = alloc_filter_coeff(LOW_PASS, HAMMING, window_len, fs, fc1, fc2);
	if (!coeff)
		return 0;

	out = (float *)malloc(sizeof(float) * 2 * length);
	if (FFT_INIT(length, DFT_1D_C2C) != NULL) {
		FFT_DFT(coeff, window_len);
		FFT_DFT_COPY(out);

		FFT_CLR();
	}
	fftw_dat_plot("sinc.dat", out, fs, length);

	free(out);
	free_filter_coeff(coeff);

	return 0;
}
