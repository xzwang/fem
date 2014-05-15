#ifndef __GOERTZEL_H__
#define __GOERTZEL_H__
#include "fft.h"

struct goertzel_algo_t {
	int length, cnt;
	double omega;
	double q0, q1, q2;
};


/*****************************************************************
* @Function	fast_goerztel_algorithm  - 快速基本goerztel算法
*
* @Param	dat - 采样原始数据
* @Param	cnt - 采样点数N，最好满足fs/cnt为整数倍
* @Param	f0 - 需要抽取幅值相位的频率
* @Param	fs - 采样频率
* @Param	fft - fft结果-赋值和相位
*
* @Returns	return 0 if success
*****************************************************************/
extern int fast_goertzel_algorithm(float *dat, int cnt, int f0, int fs, struct fft_t *fft);

extern void goertzel_init(int cnt, int f0, int fs);
extern int goertzel_update(float *dat, int cnt);
extern int goertzel_final(float *dat, int cnt, struct fft_t *fft);
#endif	/* __GOERTZEL_H__ */

