#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <pthread.h>
#include <sys/time.h>

#include "nl_msg.h"
#include "nl_pool.h"

struct fpga_msg	{
#define FPGA_MSG_HEAD	0x80
	unsigned char header;
#define CMD_FPGA_INIT	0x01
#define CMD_FPGA_RESET	0x02
#define CMD_FPGA_SUSPEND	0x03
#define CMD_FPGA_RESUME		0x04
	unsigned char cmd;
	unsigned char arg1;
	unsigned char arg2;
};

#define MSG_MAX_SIZE	2048+4

void nlmsg_poll(void)
{
	struct nlmsghdr *nlh = NULL;;
	int ret;

	while (1) {
		if (nlh == NULL)
		{
			nlh = kern_get_pool();
		}
		ret = nl_msg_recv(nlh);
		if (ret >= 0)
		{
			kern_pool_hook();
			nlh = NULL;
		}
	}
}

static pthread_t pth_nlrcv;
static pthread_attr_t pth_nlrcv_attr;

int pthread_nlrecv_create(void)
{
      struct sched_param params;
      {
              pthread_attr_init(&pth_nlrcv_attr);
              pthread_attr_setschedpolicy(&pth_nlrcv_attr, SCHED_OTHER);
              pthread_attr_getschedparam(&pth_nlrcv_attr, &params);
              params.sched_priority = sched_get_priority_max(SCHED_OTHER);
              pthread_attr_setschedparam(&pth_nlrcv_attr, &params);
              pthread_attr_setstacksize(&pth_nlrcv_attr, 262144);
      }
      return pthread_create(&pth_nlrcv, &pth_nlrcv_attr, (void *)nlmsg_poll, NULL);


}

int main(int argc, char *argv[])
{
	int ret;
	struct fpga_msg send_msg, *recv_msg;
	struct nlmsghdr *nlh;

	if (malloc_pool(MSG_MAX_SIZE) == NULL)
	{
		fprintf(stderr, "alloc pool failed\n");
		return -1;
	}

	ret = nlmsg_init();
	if (ret < 0)
	{
		fprintf(stderr, "nlmsg init failed\n");
		free_pool();
		return -1;
	}

	send_msg.header = 0x80;
	send_msg.cmd = 0x01;
	send_msg.arg1 = 0x00;
	send_msg.arg2 = 0xaa;

	pthread_nlrecv_create();

	struct timeval tv1,tv2;
	while (1) {
		send_msg.arg1++;
		gettimeofday(&tv1, NULL);
		ret = nl_msg_send(&send_msg, sizeof(struct fpga_msg));
		if (ret < 0)
		{
			fprintf(stderr, "sendmsg failed :%d\n", ret);
			free_pool();
			nlmsg_close();
			return -1;
		}
		while ((nlh = user_get_pool()) != NULL) {
			recv_msg = (struct fpga_msg *)NLMSG_DATA(nlh);
			user_pool_hook();
			gettimeofday(&tv2, NULL);
			unsigned int ms = tv2.tv_sec*1000000+tv2.tv_usec - (tv1.tv_sec*1000000+tv1.tv_usec);
			fprintf(stderr, "\n%dus, %d, 0x%x\n", ms, nlh->nlmsg_len, recv_msg->cmd);

		}

	}

	free_pool();
	nlmsg_close();

	return 0;
}
