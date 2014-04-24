#include "common.h"
#include "iio.h"
#include "fem_fb.h"

static int running = 1;
void signal_exit(void)
{
        running = 0;
        fprintf(stderr, "Signal Interrupt Happed\n");
        exit(0);
}

void signal_segv_exit(void)
{
        running = 0;
        fprintf(stderr, "Signal [%d] Interrupt Happed\n", SIGSEGV);
        exit(0);
}

int main(int argc, char *argv[])
{

	signal(SIGINT, (void *)signal_exit);
	signal(SIGQUIT, (void *)signal_exit);
	signal(SIGSEGV, (void *)signal_segv_exit);
	board_info();

	create_iio_pthread();

	while (1)
	{
		lcd_test();
	}

	stop_iio_pthread();
	return 0;
}
