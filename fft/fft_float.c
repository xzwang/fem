/***********************************************************
 * Copyright (C), 2013-2014, QiangJun Tech. Co., Ltd.
 * All Rights Reserved.
 * Filename: fft_float.c
 * Author: xz.wang
 * Last modified: 2014-05-01 10:52
 * Description:		version:
 **********************************************************/
#include "fft.h"

static struct fft_drv fft_drv;

void float_fft_clear(void)
{
	struct fft_drv *fft = &fft_drv;

	if (fft->init == 1) {
		if (!fft->dat)
			fftwf_free(fft->dat);
		if (!fft->plan)
			fftwf_destroy_plan((fftwf_plan)fft->plan);

		memset(fft, 0x0, sizeof(struct fft_drv));
	}
}

float *float_fft_init(int cnt, int flags)
{
	fftwf_complex *fftw;
	fftwf_plan plan;

	// clear fft if need
	float_fft_clear();

	switch (flags) {
		case IDFT_1D_C2C:
		case DFT_1D_C2C:
			fft_drv.length = sizeof(fftwf_complex) * cnt;
			break;
		case DFT_1D_R2C:
			fft_drv.length = sizeof(fftwf_complex) * (cnt/2 + 1);
			break;
		default:
			return NULL;
	}

	fftw = (fftwf_complex *)fftwf_malloc(fft_drv.length);
	if (!fftw) {
		return NULL;
	}

	if (flags == DFT_1D_C2C || flags == IDFT_1D_C2C) {
		plan = fftwf_plan_dft_1d(cnt, (fftwf_complex *)fftw, (fftwf_complex *)fftw, FFTW_FORWARD, FFTW_ESTIMATE);
	}
	else {
		plan = fftwf_plan_dft_r2c_1d(cnt, (float *)fftw, (fftwf_complex *)fftw, FFTW_ESTIMATE);
	}

	if (!plan)
	{
		fftwf_free(fftw);
		return NULL;
	}

	fft_drv.flags = flags;
	fft_drv.init = 1;
	fft_drv.dat = (void *)fftw;
	fft_drv.pnt = cnt;
	fft_drv.plan = (fftw_plan)plan;

	return (float *)fftw;
}

/*****************************************************************
* @Function	float_fft_dft  - 单精度DFT正变换,支持R2C/C2C
*
* @Param	dat - 原始输入数据
* @Param	cnt - DFT变换点数
*
* @Returns	非0失败,0成功
* @Note		默认不进行原位运算,即保护原始数据
*		不能使用用户_buf直接作为DFT结果buffer
*****************************************************************/
int float_fft_dft(float *dat, int cnt)
{
	struct fft_drv *fft = &fft_drv;
	float *in = NULL;

	if (fft->init != 1)
		return -1;
	if (cnt > fft->pnt || !dat) {
		return -2;
	}
	/*
	 * if (cnt <= fft->pnt)){;}-->If the actual length is short,
	 * zero padding will be used
	 */

	in = (float *)fft->dat;
	memset(in, 0x00, fft->length);

	/* 实数DFT变换 */
	if (fft->flags == DFT_1D_R2C) {
		memcpy(in, dat, sizeof(float) * cnt);
	}
	/* 复数DFT变换 */
	else {
		while (cnt--) {
			/**in++ = *dat++;*/
			/**in++ = 0x0;*/
			*in = *dat++;
			in += 2;
		}
	}

	/* fprintf(stderr, "%s\n", fftwf_export_wisdom_to_string()); */
	/* fftwf_export_wisdom_to_filename("wisdom"); */

	fftwf_execute((fftwf_plan)fft->plan);

	return 0;
}

int float_fft_idft(float *dat, int cnt)
{
	return 0;
}

int float_fft_buffer_copy(float *_buf)
{
	struct fft_drv *fft = &fft_drv;

	if (fft->init != 1)
		return -1;

	/* result : real[N][0] + imag[N][1] */
	memcpy(_buf, fft->dat, fft->length);

	return fft->length;
}

int float_dft_amp_and_phase(int fs, int f0, struct fft_t *fft_t)
{
	float K, F, P;
	float *rl, *ig;
	int oft;
	struct fft_drv *fft = &fft_drv;

	if (fft->init != 1)
		return -1;

	/* PI = 4.0*atan(1.0); P = 180/PI 弧度 */
	P = 180/(4.0*atan(1.0));
	/* DFT第0个点为直流分量,幅值=An/N, 其他点为An/(N/2) */
	K = 2.0/fft->pnt;
	/* 频率分辨率 */
	F = (float)fs/fft->pnt;

	/* f0 对应频谱点号 */
	oft = f0/F;

	rl = (float *)fft->dat + oft*2;
	ig = rl+1;

	/* 幅值 */
	fft_t->mag = sqrtf((*rl)*(*rl) + (*ig)*(*ig));
	fft_t->mag *= K;
	/* 相位 */
	fft_t->phase = atan2f(*ig, *rl);
	fft_t->phase *= P;

	return 0;
}

int fftw_data_plot(char *fname, float *dat, int fs, int cnt)
{
	FILE *fp = NULL;
	int i;
	float *rl, *ig;
	float K, An, dB, Pn;
	float freq;

	if ((fp = fopen(fname, "w+")) == NULL) {
		return -1;
	}

	fprintf(fp, "#num\tfreq\tmag\tmagdB\tphase\n");

	rl = (float *)&dat[0];
	ig = (float *)&dat[1];
	/* P = 180/(4.0 * atan(1.0)); */
	K = 2.0/cnt;

	for (i = 0; i < cnt/2; i++)
	{
		freq = (float)fs * i / cnt;
		An = sqrtf((*rl) * (*rl) + (*ig) * (*ig));
		An *= K;
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
