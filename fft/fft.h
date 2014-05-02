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

enum{
	DFT_1D_C2C,	/* 1维复数DFT变换 */
	DFT_1D_R2C,	/* 1维实数DFT变换 */
};

struct fft_drv {
	int flags;	/* 变换类型C2C/R2C */
	int init;	/* 是否被初始化 */
	int sum;	/* 变化长度 */
	void *dat;	/* 变换数据缓存,空间由type和sum决定 */
};

#define FLOAT_FFT

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
extern int float_dft_amp_and_phase(int fs, int f0, float *amp, float *phase);


extern float *double_fft_init(int cnt, int flags);
extern int double_fft_dft(float *dat, int cnt);
extern int double_fft_dft_copy(float *buf);
extern int double_fft_idft(float *dat, int cnt);
extern int double_fft_clear(void);
extern int double_dft_amp_and_phase(int fs, int f0, float *amp, float *phase);

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

	#define FFT_DFT_AMP_PHA(Fs, F0, amp, pha)	\
		float_dft_amp_and_phase(Fs, F0, amp, pha)

#elif defined(DOUBLE_FFT)
	#define TYPE_	double_
#else
	#error "UNDEFINED FFT TYPE"
#endif

#endif /* __FFT_H__ */
