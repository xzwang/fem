/***********************************************************
 * Copyright (C), 2013-2014, QiangJun Tech. Co., Ltd.
 * All Rights Reserved.
 * Filename: iio.h
 * Author: xz.wang
 * Last modified: 2014-04-23 14:53
 * Description:		version:
 **********************************************************/

#ifndef __IIO_H__
#define __IIO_H__
#include "common.h"

struct iio_struct {
	uint32_t ch;
	uint32_t *dat;
};

extern int32_t create_iio_pthread(void);
extern int32_t stop_iio_pthread(void);
#endif
