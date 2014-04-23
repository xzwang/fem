/***********************************************************
 * Filename: ringbuffer.c
 * Author: xz.wang
 * Last modified: 2013-12-19 14:18
 * Description:		version:
 **********************************************************/
//#include "common.h"
#include "ringbuffer.h"

/*****************************************************************
 * @Function	ringbuffer_init  - 缓冲队列初始化
 *
 * @Param	rb - 缓冲队列指针
 * @Param	pool - 对应的缓冲池
 * @Param	size - 缓冲池大小
 *****************************************************************/
void ringbuffer_init(struct ringbuffer *rb, void *pool, uint16 size)
{

    	/* Initialize read and write index */
    	rb->read_index = rb->write_index = 0;

   	/* Set buffer pool and size */
   	rb->buffer_ptr  = (uint8 *)pool;
    	rb->buffer_size = size;
	rb->useful_size = 0;

}

/*****************************************************************
 * @Function	ringbuffer_getchar  - 从接收队列读取一个字节
 *
 * @Param	rb - 接收队列指针
 * @Param	ch - 接收字节数据
 *
 * @Returns	0-失败 1-成功
 *****************************************************************/
int ringbuffer_getchar(struct ringbuffer *rb, uint8 *ch)
{
	/* ringbuffer is empty */
	if ( rb->useful_size == 0 )
		return 0;

	/* put character */
	*ch = rb->buffer_ptr[rb->read_index];
	rb->read_index = (rb->read_index+1) % rb->buffer_size;
	rb->useful_size--;

	return 1;
}

/*****************************************************************
 * @Function	ringbuffer_putchar  - 输出一个字节到发送队列
 *
 * @Param	rb - 发送队列指针
 * @Param	ch - 输出字节数据
 *
 * @Returns	0-失败 1-成功
 *****************************************************************/
int ringbuffer_putchar(struct ringbuffer *rb, const uint8 ch)
{

	/* whether has enough space */
	if ( rb->useful_size == rb->buffer_size)
		return 0;

	/* put character */
	rb->buffer_ptr[rb->write_index] = ch;
	rb->write_index  = (rb->write_index+1) % rb->buffer_size;
	rb->useful_size++;

	return 1;
}

/*****************************************************************
 * @Function	ringbuffer_get  - 从接收队列获取数据流
 *
 * @Param	rb - 缓冲队列指针
 * @Param	data - 用户数据
 *
 * @Returns	接收到的数据长度(0~max for buffer)
 *****************************************************************/
uint16 ringbuffer_get(struct ringbuffer *rb, void *data)
{
	uint16 count;
	uint8 *buf = (uint8 *)data;
	uint16 length;

	/* whether has enough data  */
	length = count = rb->useful_size;

	while(count--)
	{
		ringbuffer_getchar(rb, buf++);
	}

	return length;
}

/*****************************************************************
 * @Function	ringbuffer_put  - 输出数据流到发送队列
 *
 * @Param	rb - 发送队列指针
 * @Param	data - 用户数据
 * @Param	len - 用户数据长度
 *
 * @Returns	实际发送的数据长度(0~len)
 *****************************************************************/
uint16 ringbuffer_put(struct ringbuffer *rb, const void *data, uint16 len)
{
	uint16 count;
	uint8 *buf = (uint8 *)data;
	uint16 length;
	
	/* no space */
	if (rb->buffer_size - rb->useful_size < len)//buffer不足时,不发送否则在压力大时将始终发送错误的数据(只有数据头)
	//if (rb->buffer_size == rb->useful_size)
	{
		return 0;
	}

	/* whether has enough space */
	count = rb->buffer_size - rb->useful_size;

	/* drop some data */
	length = count = MIN(count, len);

	while (count--)
	{
	#if 1
		/* put character */
		rb->buffer_ptr[rb->write_index] = *buf++;
		rb->write_index  = (rb->write_index+1) % rb->buffer_size;
	#else
		ringbuffer_putchar(rb, *buf++);
	#endif
	
	}
	
	#if 1
		rb->useful_size += length;
	#endif

	return length;
}

/*****************************************************************
 * @Function	ringbuffer_useful_size  - 获取队列缓冲数据长度
 *
 * @Param	rb - 缓冲队列指针
 *
 * @Returns	缓冲区数据长度(0~N)
 *****************************************************************/
uint16 ringbuffer_useful_size(struct ringbuffer *rb)
{
    return rb->useful_size;
}
