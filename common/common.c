/***********************************************************
 * Copyright (C), 2013-2014, QiangJun Tech. Co., Ltd.
 * All Rights Reserved.
 * Filename: common.c
 * Author: xz.wang
 * Last modified: 2014-04-23 19:51
 * Description:		version:
 **********************************************************/
#include "common.h"
#include "build.h"

/*****************************************************************
* @Function	sysprintf  -格式化参数,并由system启动该命令 
*
* @Param	fmt - 可变参数
* @Param	... - 
*
* @Returns	该进程执行结果	
* @Note		sysprintf("echo %d > /sys/devices/XXX", 1);
*****************************************************************/
int32_t sysprintf(const char *fmt, ...)
{
	uint8_t cmd[64];
	va_list args;

	va_start(args, fmt);
	vsnprintf(cmd, sizeof(cmd), fmt, args);
	va_end(args);

	return system(cmd);
}

/*****************************************************************
* @Function	board_info  - display board information and 
*			software version
*
* @Returns	
*****************************************************************/
int32_t board_info(void)
{
	fprintf(stderr, "Main Process PID[%d]\n", getpid());
	fprintf(stderr, "\tDATA: %s\n", BUILD_DATE);
	fprintf(stderr, "\tVersion: %s\n", WARE_VERSION);
}

/*****************************************************************
* @Function	sysdate  - get system time, UTC is 0
*
* @Param	tm - the pointer to the tm
*
* @Returns	0
*****************************************************************/
int32_t systime(struct tm *tm)
{
        time_t timep;

        time(&timep);

//      UTC date
//      tm = gmtime(&timep);

        /* local date */
        tm = localtime(&timep);

        return 0;
}

/*****************************************************************
* @Function	get_of_tv  -
*
* @Returns
*****************************************************************/
uint32_t get_of_tv(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec*100000+tv.tv_usec;
}


