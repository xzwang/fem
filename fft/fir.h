/***********************************************************
 * Copyright (C), 2013-2014, QiangJun Tech. Co., Ltd.
 * All Rights Reserved.
 * Filename: firfilter.h
 * Author: xz.wang
 * Last modified: 2014-05-07 15:30
 * Description:		version:
 **********************************************************/
#ifndef __FIR_FILTER_H__
#define __FIR_FILTER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* filtels type */
#define	LOW_PASS	0x10
#define HIGH_PASS	0x20
#define BAND_PASS	0x40
#define BAND_STOP	0x80

extern double *double_create_sinc(int length, int f0, int fs, int filtertype);
extern int double_create_window(int type, int length, double **coefficient);

extern int float_fir_filter(float *in, float *out, int length, float *coeff, int coeff_num);
extern float *alloc_filter_coeff(int filter_type, int window_type, int length, int fs, int fc1, int fc2);
extern void free_filter_coeff(float *coeff);

/* Windows type */
#define	RECTANGULAR	1
#define	HANNING		2
#define	HAMMING		3
#define	BLACKMAN	4
#define	BARTLETT	5

#endif	/* __FIR_FILTER_H__ */
