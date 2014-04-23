#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include "types.h"

/* ring buffer */
typedef struct ringbuffer
{
    	uint16 read_index, write_index;
    	uint8 *buffer_ptr;
    	uint16 buffer_size;
	uint16 useful_size;
}stRingBuffer,*pstRingBuffer;


/*****************************************************************
 * @Function	ringbuffer_init  - 缓冲队列初始化
 *
 * @Param	rb - 缓冲队列指针
 * @Param	pool - 对应的缓冲池
 * @Param	size - 缓冲池大小
 *****************************************************************/
void ringbuffer_init(struct ringbuffer *rb, void *pool, uint16 size);

/*****************************************************************
 * @Function	ringbuffer_getchar  - 从接收队列读取一个字节
 *
 * @Param	rb - 接收队列指针
 * @Param	ch - 接收字节数据
 *
 * @Returns	0-失败 1-成功
 *****************************************************************/
int ringbuffer_getchar(struct ringbuffer *rb, uint8 *ch);

/*****************************************************************
 * @Function	ringbuffer_putchar  - 输出一个字节到发送队列
 *
 * @Param	rb - 发送队列指针
 * @Param	ch - 输出数据
 *
 * @Returns	0-失败 1-成功
 *****************************************************************/
int ringbuffer_putchar(struct ringbuffer *rb, const uint8 ch);

/*****************************************************************
 * @Function	ringbuffer_get  - 从接收队列获取数据流
 *
 * @Param	rb - 缓冲队列指针
 * @Param	data - 用户数据
 *
 * @Returns	接收到的数据长度(0~max for buffer)
 *****************************************************************/
 uint16 ringbuffer_get(struct ringbuffer *rb, void *data);
 
/*****************************************************************
 * @Function	ringbuffer_put  - 输出数据流到发送队列
 *
 * @Param	rb - 发送队列指针
 * @Param	data - 用户数据
 * @Param	len - 用户数据长度
 *
 * @Returns	实际发送的数据长度(0~len)
 *****************************************************************/
 uint16 ringbuffer_put(struct ringbuffer *rb, const void *data, uint16 len);
 
/*****************************************************************
 * @Function	ringbuffer_useful_size  - 获取队列缓冲数据长度
 *
 * @Param	rb - 缓冲队列指针
 *
 * @Returns	缓冲区数据长度(0~N)
 *****************************************************************/
uint16 ringbuffer_useful_size(struct ringbuffer *rb);
 
#endif /* _RING_BUFFER_H_ */

