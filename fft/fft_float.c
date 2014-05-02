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
static fftwf_plan plan;

enum {
	F_NONE = 0,
	F_INIT,
	F_DFT,
	F_IDFT,
	F_END,
};

/*****************************************************************
* @Function	float_fft_init  - 初始化FFT-DFT参数,分配内存
*
* @Param	cnt - DFT点数
* @Param	flags - DFT类型
*
* @Returns	用户buffer
*****************************************************************/
float *float_fft_init(int cnt, int flags)
{
	fftwf_complex *fftw;

	if (fft_drv.init > F_NONE && fft_drv.dat != NULL) {
		fftwf_free(fft_drv.dat);
	}
	if (flags == DFT_1D_C2C) {
		fftw = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * cnt);
	}
	else if (flags == DFT_1D_R2C) {
		fftw = fftwf_malloc(sizeof(fftwf_complex) * (cnt/2 + 1));
	}
	else {
		fprintf(stderr, "%s flags invaild\n", __FUNCTION__);
		return NULL;
	}
	if (fftw == NULL) {
		fprintf(stderr, "fftw memory allocation failure\n");
		return NULL;
	}

	fft_drv.flags = flags;
	fft_drv.init = F_INIT;
	fft_drv.dat = (void *)fftw;
	fft_drv.sum = cnt;

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
	int n = cnt;

	if (fft->init == F_NONE) {
		return -1;
	}
	if (!dat || cnt != fft->sum) {

		return -2;
	}

	/* 实数DFT变换 */
	if (fft->flags == DFT_1D_R2C) {
		plan = fftwf_plan_dft_r2c_1d(cnt, dat, (fftwf_complex *)fft->dat, FFTW_ESTIMATE);
	}
	/* 复数DFT变换 */
	else if (fft->flags == DFT_1D_C2C) {
		in = (float *)fft->dat;
		while (n--) {
			*in++ = *dat++;
			*in++ = 0x0;
		}
		plan = fftwf_plan_dft_1d(cnt, (fftwf_complex *)fft->dat, (fftwf_complex *)fft->dat, FFTW_FORWARD, FFTW_ESTIMATE);
	}
	else {
		return -3;
	}
	fft->init = F_DFT;

	fftwf_execute(plan);

	fftwf_destroy_plan(plan);

	return 0;
}

int float_fft_idft(float *dat, int cnt)
{
	return 0;
}

int float_fft_dft_copy(float *_buf)
{
	struct fft_drv *fft = &fft_drv;
	int length;

	if (fft->init != F_DFT || !_buf)
		return -1;
	/* 实部 + 虚部 */
	length = fft->sum * 2;

	/* 点数 * 每个点空间 */
	memcpy(_buf, fft->dat, length * sizeof(float));

	return length;
}

int float_fft_clear(void)
{
	struct fft_drv *fft = &fft_drv;

	if (fft->dat != NULL) {
		fftwf_free(fft->dat);
	}

	/* fftwf_destroy_plan(plan); */
	memset(fft, 0x0, sizeof(struct fft_drv));

	return 0;
}

int float_dft_amp_and_phase(int fs, int f0, float *amp, float *phase)
{
	float P, K, F;
	float *rl, *ig;
	int n;
	struct fft_drv *fft = &fft_drv;

	if (fft->init != F_DFT) {
		return -1;
	}

	/* PI = 4.0*atan(1.0); P = 180/PI 弧度 */
	P = 180/(4.0*atan(1.0));
	/* DFT第0个点为直流分量,幅值=An/N, 其他点为An/(N/2) */
	K = (float)2/fft->sum;
	/* 频率分辨率 */
	F = (float)fs/fft->sum;

	/* f0 对应频谱点号 */
	n = (int)(f0/F);

	rl = (float *)fft->dat + n*2;
	ig = rl+1;


	/* 幅值 */
	*amp = sqrtf((*rl)*(*rl) + (*ig)*(*ig));
	*amp *= K;
	/* 相位 */
	*phase = atan2f(*ig, *rl);
	*phase *= P;

	return 0;
}


