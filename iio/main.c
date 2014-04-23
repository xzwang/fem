#include "common.h"
#include "iio.h"

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

	FEM_DBG("Main Process:PID[%d]\n", getpid());
	signal(SIGINT, (void *)signal_exit);
	signal(SIGQUIT, (void *)signal_exit);
	signal(SIGSEGV, (void *)signal_segv_exit);

	create_iio_pthread();
	stop_iio_pthread();
	
	return 0;
}