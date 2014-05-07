/***********************************************************
 * Copyright (C), 2013-2014, QiangJun Tech. Co., Ltd.
 * All Rights Reserved.
 * Filename: fft.h
 * Author: xz.wang
 * Last modified: 2014-04-30 18:06
 * Description:		version:
 **********************************************************/
#ifndef __FFT_H__
#define __FFT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fftw3.h"

#define FLOAT_FFT


struct fft_drv {
	int flags;	/* 变换类型C2C/R2C */
	int init;	/* 是否被初始化 */
	int sum;	/* 变化长度 */
	void *dat;	/* 变换数据缓存,空间由type和sum决定 */
};

/* DFT types */
#define DFT_1D_C2C	0x01	/* 1维复数DFT变换 */
#define DFT_1D_R2C	0x02	/* 1维实数DFT变换 */

/* FFT process */
#define F_NONE	0x00
#define F_INIT	0x01
#define	F_DFT	0x02
#define	F_IDFT	0x03
#define	F_END	0x04

struct fft_t {
#ifdef FLOAT_FFT
	float mag;
	float phase;
#else
	double mag;
	double phase;
#endif
};


/*
 * FLOAT FFT	--单精度FFT-DFT变化
 * 根据实际情况单精度DFT变化已满足精度需要,关于数据的输出有两种方式:
 * 1.上层维护一个_buf指针,大小及分配在DFT变换过程初始化,指向DFT变换结果
 * 内存,这样可以避免大数据的拷贝,上层可以直接操作DFT变换后的结果,但不安全
 * 2.向用户完全屏蔽DFT变化细节,上层自行分配拷贝DFT结构,这需要上层自行计算
 * DFT结果大小，通过FFT_DFT_COPY获取数据
 */
extern float *float_fft_init(int cnt, int flags);
extern int float_fft_dft(float *dat, int cnt);
extern int float_fft_dft_copy(float *buf);
extern int float_fft_idft(float *dat, int cnt);
extern int float_fft_clear(void);
extern int float_dft_amp_and_phase(int fs, int f0, struct fft_t *fft_t);
extern int float_fast_goerztel_algorithm(float *dat, int cnt, int f0, int fs, struct fft_t *fft_t);
extern int fftw_data_plot(char *fname, float *dat, int fs, int cnt);

extern double *double_fft_init(int cnt, int flags);
extern int double_fft_dft(double *dat, int cnt);
extern int double_fft_dft_copy(double *buf);
extern int double_fft_idft(double *dat, int cnt);
extern int double_fft_clear(void);
extern int double_dft_amp_and_phase(int fs, int f0, struct fft_t *fft);
extern int double_fast_goerztel_algorithm(double *dat, int cnt, int f0, int fs, struct fft_t *fft_t);

#ifdef FLOAT_FFT
	#define FFT_DFT(dat, cnt)	\
		float_fft_dft(dat, cnt)

	#define FFT_IDFT(buf, cnt)	\
		float_fft_idft(buf, cnt)

	#define FFT_INIT(cnt, flags)	\
		float_fft_init(cnt, flags)

	#define FFT_CLR()	\
		float_fft_clear()

	#define FFT_DFT_COPY(buf)	\
		float_fft_dft_copy(buf)

	#define FFT_DFT_AMP_PHA(Fs, F0, fft_t)	\
		float_dft_amp_and_phase(Fs, F0, fft_t)

	#define FAST_GOERZTEL_DFT(dat, cnt, f0, fs, fft_t)	\
		float_fast_goerztel_algorithm(dat, cnt, f0, fs, fft_t)

#elif defined(DOUBLE_FFT)
	#define TYPE_	double_
#else
	#error "UNDEFINED FFT TYPE"
#endif

#endif /* __FFT_H__ */
