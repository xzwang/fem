#ifndef __NL_MSG_H__
#define __NL_MSG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>


#define NETLINK_FPGA	22

static inline struct nlmsghdr *nlmsg_malloc(int length)
{
	return (struct nlmsghdr *)malloc(NLMSG_SPACE(length));
}

static inline void nlmsg_free(struct nlmsghdr *p)
{
	free(p);
}

static inline void *nlmsg_data(struct nlmsghdr *p)
{
	return NLMSG_DATA(p);
}

static inline int nlmsg_datalen(struct nlmsghdr *p)
{
	return p->nlmsg_len - NLMSG_SPACE(0);
}

extern int nlmsg_init();
extern void nlmsg_close(void);


int nl_msg_send(void *data, int length);
int nl_msg_recv(struct nlmsghdr *nlh);

#endif /* __NL_MSG_H__ */
