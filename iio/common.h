/***********************************************************
 * Copyright (C), 2013-2014, QiangJun Tech. Co., Ltd.
 * All Rights Reserved.
 * Filename: common.h
 * Author: xz.wang
 * Last modified: 2014-04-23 14:38
 * Description:		version:
 **********************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <paths.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <syslog.h>
#include <signal.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>

#define DEBUG 2
#if (defined(DEBUG) && DEBUG > 1)
	#define FEM_DBG(fmt, args...)	\
		do { fprintf(stderr, fmt, ##args);}while(0)
#else
	#define FEM_DBG(fmt. args...)	\
		do {;}while(0)
#endif

#endif /* __COMMON_H__ */
