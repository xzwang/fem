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

/*****************************************************************
* @Function	float_fir_filter  - FIR digital filter
*
* @Param	in - signal in
* @Param	out - signal out
* @Param	length - signal length
* @Param	coeff - filter coefficient
* @Param	coeff_num - filter coefficient length
*
* @Returns	0-success,otherwise failed
*****************************************************************/
extern int float_fir_filter(float *in, float *out, int length, float *coeff, int coeff_num);

/*****************************************************************
* @Function	alloc_filter_coeff  - create FIR filtel coefficient
*
* @Param	filter_type - low/high-pass band-pass/stop
* @Param	window_type - hamming/hanning...,etc
* @Param	length - FIR filtel coefficient N=order+1
* @Param	fs - sample frequency
* @Param	fc1 - cut-off frequency (low)
* @Param	fc2 - cut-off frequency (high)
*
* @Returns	NULL-failed, otherwise return the firtel coeff point
*****************************************************************/
extern float *alloc_filter_coeff(int filter_type, int window_type, int length, int fs, int fc1, int fc2);

/*****************************************************************
* @Function	free_filter_coeff  - free firtel coefficient
*
* @Param	coeff - assigned firtel coefficient
*****************************************************************/
extern void free_filter_coeff(float *coeff);

/* Windows type */
#define	RECTANGULAR	1
#define	HANNING		2
#define	HAMMING		3
#define	BLACKMAN	4
#define	BARTLETT	5

#endif	/* __FIR_FILTER_H__ */
