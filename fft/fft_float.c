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
		memset(fftw, 0x00, sizeof(fftwf_complex)*cnt);
	}
	else if (flags == DFT_1D_R2C) {
		fftw = fftwf_malloc(sizeof(fftwf_complex) * (cnt/2 + 1));
		memset(fftw, 0x00, sizeof(fftwf_complex) * (cnt/2 + 1));
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
	fftwf_plan plan;

	if (fft->init == F_NONE) {
		return -1;
	}

	if (cnt > fft->sum || !dat) {
		return -2;
	}
	/*
	 * if (cnt <= fft->sum)){;}-->If the actual length is short,
	 * zero padding will be used
	 */

	/* 实数DFT变换 */
	if (fft->flags == DFT_1D_R2C) {
		plan = fftwf_plan_dft_r2c_1d(fft->sum, dat, (fftwf_complex *)fft->dat, FFTW_ESTIMATE);
		if (!plan) {
			return -3;
		}
	}
	/* 复数DFT变换 */
	else if (fft->flags == DFT_1D_C2C) {
		in = (float *)fft->dat;
		while (cnt--) {
			*in++ = *dat++;
			*in++ = 0x0;
		}
		plan = fftwf_plan_dft_1d(fft->sum, (fftwf_complex *)fft->dat, (fftwf_complex *)fft->dat, FFTW_FORWARD, FFTW_ESTIMATE);
	}
	else {
		return -3;
	}
	fft->init = F_DFT;
	/* fprintf(stderr, "%s\n", fftwf_export_wisdom_to_string()); */
	/* fftwf_export_wisdom_to_filename("wisdom"); */

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

	memset(fft, 0x0, sizeof(struct fft_drv));

	return 0;
}

int float_dft_amp_and_phase(int fs, int f0, struct fft_t *fft_t)
{
	float K, F;
	float *rl, *ig;
	int oft;
	struct fft_drv *fft = &fft_drv;

	if (fft->init != F_DFT) {
		return -1;
	}

	/* PI = 4.0*atan(1.0); P = 180/PI 弧度 */
	/* P = 180/(4.0*atan(1.0)); */
	/* DFT第0个点为直流分量,幅值=An/N, 其他点为An/(N/2) */
	K = 2.0/fft->sum;
	/* 频率分辨率 */
	F = (float)fs/fft->sum;

	/* f0 对应频谱点号 */
	oft = f0/F;

	rl = (float *)fft->dat + oft*2;
	ig = rl+1;


	/* 幅值 */
	fft_t->mag = sqrtf((*rl)*(*rl) + (*ig)*(*ig));
	fft_t->mag *= K;
	/* 相位 */
	fft_t->phase = atan2f(*ig, *rl);
	/* *phase *= P; */

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
	float PI, K;

	k = (float)(cnt * f0)/fs;

	PI = (4.0*atan(1.0));
	K = 2.0/cnt;
	omega = (2.0 * PI * k)/cnt;

	/* 进行一次正弦和余弦计算 */
	sine = sinf(omega);
	cosine = cosf(omega);
	/* 计算系数 */
	coeff = 2.0 * cosine;

	q0 = q1 = q2 = 0.0;

	for ( i = 0; i < cnt; i++) {
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

	return 0;
}


int fftw_data_plot(char *fname, float *dat, int fs, int cnt)
{
	FILE *fp = NULL;
	int i;
	float *rl, *ig;
	float K,An, dB, Pn;
	float freq;

	if ((fp = fopen(fname, "w+")) == NULL) {
		return -1;
	}

	fprintf(fp, "#num\tfreq\tmag\tmagdB\tphase\n");

	rl = (float *)&dat[0];
	ig = (float *)&dat[1];
	/* P = 180/(4.0 * atan(1.0)); */
	K = (float)2/cnt;

	for (i = 0; i < cnt; i++)
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
