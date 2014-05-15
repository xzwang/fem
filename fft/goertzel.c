#include "goertzel.h"


/*
 * 关于goertzel 算法,主要用于快速向量计算和快速谐波提取。虽然FFT在频谱分析中
 * 应用更加广泛，但如果只是计算基波或少量谐波，就没有必要进行全谱分析，无论是
 * 从速度上还是内存开销上，goertzel算法更加有效。另外FFT必须等数据完整接收才能
 * 进行，而goertzel则可以采用滑动窗口方法将数据分割计算
 * goertzel误差:
 * 1.系数coeff 2.乘法截断 3.加法舍入
 */

static struct goertzel_algo_t gzl_algo;

/*****************************************************************
* @Function	fast_goertzel_algorithm  - 快速基本goertzel算法
*
* @Param	dat - 采样原始数据
* @Param	cnt - 采样点数N，最好满足fs/cnt为整数倍
* @Param	f0 - 需要抽取幅值相位的频率
* @Param	fs - 采样频率
* @Param	fft - fft结果-赋值和相位
*
* @Returns	return 0 if success
*****************************************************************/
int fast_goertzel_algorithm(float *dat, int cnt, int f0, int fs, struct fft_t *fft)
{
	double omega, sine, cosine, coeff;
	double q0, q1, q2, rl, ig;
	int i;
	double PI, K, P;

	PI = 4.0 * atan(1.0);
	K = 2.0/cnt;	// A = An/(2/N)
	P = 180.0/PI;	// phase angle

	/*
	 * coeff = 2.0 * cos((2.0 * PI * k)/GOERTZEL_N)
	 * where k = (int) (0.5 + (GOERTZEL_N * target_freq)/SAMPLE_RATE)
	 *
	 * More simply: coeff = 2.0 * cos(2.0 * PI * target_freq/SAMPLE_RATE)
	 */

	/* k = (double)(0.5 + cnt * f0)/fs; */
	/* omega = (2.0 * PI * k)/cnt; */
	omega = (2.0 * PI * f0/fs);

	sine = sin(omega);
	cosine = cos(omega);
	coeff = 2.0 * cosine;

	q1 = q2 = 0.0;

	for (i = 0; i < cnt; i++) {
		q0 = dat[i] + coeff*q1 - q2;
		q2 = q1;
		q1 = q0;
	}
	/* double m = q1*q1 + q2*q2 - coeff*q1*q2; */
	/* m = sqrt(m) * K; */
	/* fprintf(stderr, "power = %lf\n", m); */

	/* rl = q1 - q2 * cosine; */
	/* ig = q2 * sine; */
	rl = cosine * q1 - q2;
	ig = sine * q1;

	fft->mag = sqrtf(rl*rl + ig*ig);
	fft->mag *= K;
	fft->phase = atan2f(ig, rl);
	fft->phase *= P;

	return 0;
}

void goertzel_init(int cnt, int f0, int fs)
{
	double PI = 4.0 * atan(1.0);
	/* double k = (double)(cnt * f0) / fs; */
	/* double omega = (2.0 * PI * k) / cnt; */
	double omega = 2.0 * PI * f0/fs;

	memset(&gzl_algo, 0x0, sizeof(struct goertzel_algo_t));

	gzl_algo.length = cnt;
	gzl_algo.omega = omega;

}

int goertzel_update(float *dat, int cnt)
{
	int i;
	struct goertzel_algo_t *gzl = &gzl_algo;
	double coeff = 2.0 * cos(gzl->omega);

	if (gzl->cnt < gzl->length) {
		gzl->cnt += cnt;

		for (i = 0; i < cnt; i++) {
			gzl->q0 = coeff * gzl->q1 - gzl->q2 + *dat++;
			gzl->q2 = gzl->q1;
			gzl->q1 = gzl->q0;
		}

		return 0;
	}
	else {
		return -1;
	}
}

int goertzel_final(float *dat, int cnt, struct fft_t *fft)
{
	double rl, ig, P;
	int i;
	struct goertzel_algo_t *gzl = &gzl_algo;
	double coeff = 2.0 * cos(gzl->omega);
	double sine;

	if (cnt > 0 && dat) {
		gzl->cnt += cnt;

		for (i = 0; i < cnt; i++) {
			gzl->q0 = coeff * gzl->q1 - gzl->q2 + *dat++;
			gzl->q2 = gzl->q1;
			gzl->q1 = gzl->q0;
		}
	}

	if (gzl->cnt == gzl->length) {
		sine = sin(gzl->omega);

		P = 180.0/(4.0*atan(1.0));

		rl = 0.5 * gzl->q1 * coeff - gzl->q2;
		ig = gzl->q1 * sine;

		fft->mag = sqrtf(rl*rl + ig*ig);
		fft->mag *= (2.0/gzl->cnt);
		fft->phase = atan2f(ig, rl);
		fft->phase *= P;

		return 0;
	}
	else {
		return -1;
	}
}
