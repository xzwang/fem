#include "nl_pool.h"

#define GROUP_NUM	16

struct nl_pool {
	struct nlmsghdr *nlh[GROUP_NUM];
	unsigned int in, out;
	unsigned int useful;
	unsigned int space;
	int lock;
};

static struct nl_pool pool;

void *malloc_pool(unsigned int space)
{
	int i, j;
	struct nlmsghdr *nlh;

	pool.in = pool.out = 0;
	pool.useful = 0;
	pool.space = space;
	pool.lock = 1;

	for (i = 0; i < GROUP_NUM; i++)
	{
		nlh = nlmsg_malloc(space);
		if (nlh != NULL)
			pool.nlh[i] = nlh;
		else
			break;
	}

	if (i != GROUP_NUM)
	{
		for (j = 0; j < i; j++)
			free(pool.nlh[j]);

		return NULL;
	}

	return &pool;
}

void free_pool(void)
{
	int i;

	for (i = 0; i < GROUP_NUM; i++)
	{
		free(pool.nlh[i]);
	}

	memset(&pool, 0x0, sizeof(struct nl_pool));
}

// userspace get nlmsghdr from pool
void user_pool_hook(void)
{
	while (!pool.lock) {;}	//spin_lock
	pool.lock--;
	pool.out = (pool.out+1) % GROUP_NUM;
	pool.useful--;
	pool.lock++;
}

struct nlmsghdr *user_get_pool(void)
{
	if (pool.useful == 0)
		return NULL;

	/* put a pool */
	return  pool.nlh[pool.out];
}

// kernel get nlmsghdr from pool
void kern_pool_hook(void)
{
	while (!pool.lock) {;}	//spin_lock
	pool.lock--;
	pool.in = (pool.in + 1) % GROUP_NUM;
	pool.useful++;
	pool.lock++;
}

struct nlmsghdr *kern_get_pool(void)
{
	struct nlmsghdr *nlh = NULL;

	/* pool is full */
	if (pool.useful == GROUP_NUM)
		return NULL;

	nlh = pool.nlh[pool.in];
	memset(nlh, 0, sizeof(struct nlmsghdr));
	nlh->nlmsg_pid = getpid();
	nlh->nlmsg_len = NLMSG_LENGTH(pool.space);

	/* get a pool */
	return nlh;
}
