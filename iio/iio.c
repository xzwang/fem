/***********************************************************
 * Copyright (C), 2013-2014, QiangJun Tech. Co., Ltd.
 * All Rights Reserved.
 * Filename: iio.c
 * Author: xz.wang
 * Last modified: 2014-04-23 14:37
 * Description:		version:
 **********************************************************/

#include "iio.h"

static pthread_t pth_iio;
static pthread_attr_t  pth_iio_attr;

#define IIO_DEV "/sys/bus/iio/devices/iio:device0/in_voltage4_raw"

int32_t iio_raw_read(void)
{
	FILE *fp = NULL;	
	int32_t err = -1;
	char buf[6];
	int32_t value = 0xff;

	fp = fopen(IIO_DEV, "r");
	if (fp == NULL) {
		fprintf(stderr, "fopen %s file failed\n", IIO_DEV);
		return -1;
	}
	
	memset(buf, 0x0, sizeof(buf));
	err = fread(buf, 1, 5, fp);
	if (err < 0) {
		fprintf(stderr, "%s read error\n", __FUNCTION__);
		fclose(fp);
		return -2;
	}

	value = atoi(buf);
	fclose(fp);

	return value;
}

void iio_pthread(void)
{
	int32_t dat;
	float resol = ((float)1800)/4095, voltage;//mV

	fprintf(stderr, "resolution %f", resol);
	while (1) {
	
		dat = iio_raw_read();
		voltage = resol * dat;

		FEM_DBG("%f mV\n", voltage);
		/* usleep(10000); */
		sleep(1);
	}
}

int32_t create_iio_pthread(void)
{
	struct sched_param iio_params;
	
	/*
	 * Create IIO ADC sample pthread
	 */
	pthread_attr_init(&pth_iio_attr);
	pthread_attr_setschedpolicy(&pth_iio_attr, SCHED_OTHER);
	pthread_attr_getschedparam(&pth_iio_attr, &iio_params);
	iio_params.sched_priority = sched_get_priority_max(SCHED_OTHER);
	pthread_attr_setschedparam(&pth_iio_attr, &iio_params);
	pthread_attr_setstacksize(&pth_iio_attr, 262144);	//stack 256K bytes

	return pthread_create(&pth_iio, &pth_iio_attr, (void *)iio_pthread, NULL);
}

int32_t stop_iio_pthread(void)
{
	pthread_join(pth_iio, NULL);
	
	return 0; 
}
