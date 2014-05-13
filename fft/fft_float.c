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

/*
 * 关于goerztel 算法,主要用于快速向量计算和快速谐波提取。虽然FFT在频谱分析中
 * 应用更加广泛，但如果只是计算基波或少量谐波，就没有必要进行全谱分析，无论是
 * 从速度上还是内存开销上，goerztel算法更加有效。另外FFT必须等数据完整接收才能
 * 进行，而goerztel则可以采用滑动窗口方法将数据分割计算
 * goerztel误差:
 * 1.系数coeff 2.乘法截断 3.加法舍入
 */
/*****************************************************************
* @Function	float_fast_goerztel_algorithm  - 快速基本goerztel算法
*
* @Param	dat - 采样原始数据
* @Param	cnt - 采样点数N，最好满足fs/cnt为整数倍
* @Param	f0 - 需要抽取幅值相位的频率
* @Param	fs - 采样频率
* @Param	fft - fft结果-赋值和相位
*
* @Returns	return 0 if success
*****************************************************************/
int float_fast_goerztel_algorithm(float *dat, int cnt, int f0, int fs, struct fft_t *fft)
{
	float omega, sine, cosine, coeff;
	float k, q0, q1, q2, rl, ig;
	int i;
	float PI, K, P;

	k = (float)(cnt * f0)/fs;

	PI = (4.0*atan(1.0));
	K = 2.0/cnt;
	P = 180/(4.0*atan(1.0));
	omega = (2.0 * PI * k)/cnt;

	/* 进行一次正弦和余弦计算 */
	sine = sinf(omega);
	cosine = cosf(omega);
	/* 计算系数 */
	coeff = 2.0 * cosine;

	q0 = q1 = q2 = 0.0;

	for (i = 0; i < cnt; i++) {
		q0 = coeff*q1 - q2 + dat[i];
		q2 = q1;
		q1 = q0;
	}

	rl = (q1 - q2*cosine);
	ig = (q2*sine);

	/* 计算模值和相位 */
	fft->mag = sqrtf(rl*rl + ig*ig);
	fft->mag *= K;
	fft->phase = atan2f(ig, rl);
	fft->phase *= P;
	return 0;
}

struct goerztel_algo_t {
	int length, cnt;
	float sine;
	float cosine;
	float coeff;
	float q0,q1,q2;
};

struct goerztel_algo_t gzl_algo;

void float_goerztel_init(int cnt, int f0, int fs)
{
	float PI = 4.0 * atan(1.0);
	float k = (float)(cnt * f0) / fs;
	float omega = (2.0 * PI * k) / cnt;

	memset(&gzl_algo, 0x0, sizeof(struct goerztel_algo_t));

	gzl_algo.length = cnt;

	/* 进行一次正弦和余弦计算 */
	gzl_algo.sine = sinf(omega);
	gzl_algo.cosine = cosf(omega);
	/* 计算系数 */
	gzl_algo.coeff = 2.0 * gzl_algo.cosine;
}

int float_goerztel_update(float *dat, int cnt)
{
	int i;
	struct goerztel_algo_t *gzl = &gzl_algo;

	if (gzl->cnt < gzl->length) {
		gzl->cnt += cnt;

		for (i = 0; i < cnt; i++) {
			gzl->q0 = gzl->coeff * gzl->q1 - gzl->q2 + *dat++;
			gzl->q2 = gzl->q1;
			gzl->q1 = gzl->q0;
		}

		return 0;
	}
	else {
		return -1;
	}
}

int float_goerztel_final(float *dat, int cnt, struct fft_t *fft)
{
	float rl, ig, P;
	struct goerztel_algo_t *gzl = &gzl_algo;
	int i;

	if (cnt > 0 && dat) {
		gzl->cnt += cnt;

		for (i = 0; i < cnt; i++) {
			gzl->q0 = gzl->coeff * gzl->q1 - gzl->q2 + *dat++;
			gzl->q2 = gzl->q1;
			gzl->q1 = gzl->q0;
		}
	}

	P = 180/(4.0*atan(1.0));
	if (gzl->cnt == gzl->length) {

		rl = (gzl->q1 - gzl->q2 * gzl->cosine);
		ig = (gzl->q2 * gzl->sine);

		/* 计算模值和相位 */
		fft->mag = sqrtf(rl*rl + ig*ig);
		fft->mag *= (2.0/gzl->cnt);
		fft->phase = atan2f(ig, rl);
		fft->phase *= P;

		return 0;
	}
	else {
		return -1;
	}
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
