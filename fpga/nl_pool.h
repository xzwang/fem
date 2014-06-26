#ifndef __NL_POOL_
#define __NL_POOL_
#include "nl_msg.h"

extern void *malloc_pool(unsigned int space);
extern void free_pool(void);

extern void kern_pool_hook(void);
extern struct nlmsghdr *kern_get_pool(void);

extern void user_pool_hook(void);
extern struct nlmsghdr *user_get_pool(void);

#endif
