#include "nl_msg.h"

struct nl_sock {
	int fd;
	struct sockaddr_nl sa, da;
	struct msghdr msg;
};

static struct nl_sock sk;


int nl_msg_send(void *data, int length)
{
	int ret = -1;
	struct nlmsghdr *nlh;


	nlh = nlmsg_malloc(length);
	if (nlh == NULL)
	{
		return -1;
	}

	memset(nlh, 0, sizeof(struct nlmsghdr));
	nlh->nlmsg_pid = getpid();

	sk.msg.msg_iov->iov_base = (void *)nlh;
	sk.msg.msg_iov->iov_len = nlh->nlmsg_len = NLMSG_LENGTH(length);

	memcpy(NLMSG_DATA(nlh), data, length);

	ret = sendmsg(sk.fd, &sk.msg, 0);

	free(nlh);

	return ret;
}

int nl_msg_recv(struct nlmsghdr *nlh)
{

	sk.msg.msg_iov->iov_base = (void *)nlh;
	sk.msg.msg_iov->iov_len = nlh->nlmsg_len;

	return recvmsg(sk.fd, &sk.msg, 0);
}

int nlmsg_init(void)
{
	int fd;
	struct iovec *iov;

	memset(&sk, 0x0, sizeof(struct nl_sock));

	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_FPGA);
	if (fd < 0)
	{
		return -1;
	}
	sk.fd = fd;

	memset(&sk.sa, 0x0, sizeof(struct sockaddr_nl));
	memset(&sk.da, 0x0, sizeof(struct sockaddr_nl));

	sk.sa.nl_family = AF_NETLINK;
	sk.sa.nl_pid = getpid();
	sk.sa.nl_groups = 0x0;

	sk.da.nl_family = AF_NETLINK;
	sk.da.nl_pid = 0;
	sk.da.nl_groups = 0x0;

	bind(fd, (struct sockaddr *)&sk.sa, sizeof(struct sockaddr_nl));

	iov = (struct iovec *)malloc(sizeof(struct iovec));
	if (iov == NULL) {
		close(fd);
		return -2;
	}

	sk.msg.msg_iov = iov;
	sk.msg.msg_iovlen = 1;

	sk.msg.msg_name = (void *)&sk.da;
	sk.msg.msg_namelen = sizeof(struct sockaddr_nl);

	return fd;
}

void nlmsg_close(void)
{
	free(sk.msg.msg_iov);
	close(sk.fd);
	memset(&sk, 0x0, sizeof(struct nl_sock));
}
