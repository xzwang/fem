/***********************************************************
 * Filename: ringbuffer.c
 * Author: xz.wang
 * Last modified: 2013-12-19 14:18
 * Description:		version:
 **********************************************************/
//#include "common.h"
#include "ringbuffer.h"

/*****************************************************************
 * @Function	ringbuffer_init  - ������г�ʼ��
 *
 * @Param	rb - �������ָ��
 * @Param	pool - ��Ӧ�Ļ����
 * @Param	size - ����ش�С
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
 * @Function	ringbuffer_getchar  - �ӽ��ն��ж�ȡһ���ֽ�
 *
 * @Param	rb - ���ն���ָ��
 * @Param	ch - �����ֽ�����
 *
 * @Returns	0-ʧ�� 1-�ɹ�
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
 * @Function	ringbuffer_putchar  - ���һ���ֽڵ����Ͷ���
 *
 * @Param	rb - ���Ͷ���ָ��
 * @Param	ch - ����ֽ�����
 *
 * @Returns	0-ʧ�� 1-�ɹ�
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
 * @Function	ringbuffer_get  - �ӽ��ն��л�ȡ������
 *
 * @Param	rb - �������ָ��
 * @Param	data - �û�����
 *
 * @Returns	���յ������ݳ���(0~max for buffer)
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
 * @Function	ringbuffer_put  - ��������������Ͷ���
 *
 * @Param	rb - ���Ͷ���ָ��
 * @Param	data - �û�����
 * @Param	len - �û����ݳ���
 *
 * @Returns	ʵ�ʷ��͵����ݳ���(0~len)
 *****************************************************************/
uint16 ringbuffer_put(struct ringbuffer *rb, const void *data, uint16 len)
{
	uint16 count;
	uint8 *buf = (uint8 *)data;
	uint16 length;
	
	/* no space */
	if (rb->buffer_size - rb->useful_size < len)//buffer����ʱ,�����ͷ�����ѹ����ʱ��ʼ�շ��ʹ��������(ֻ������ͷ)
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
 * @Function	ringbuffer_useful_size  - ��ȡ���л������ݳ���
 *
 * @Param	rb - �������ָ��
 *
 * @Returns	���������ݳ���(0~N)
 *****************************************************************/
uint16 ringbuffer_useful_size(struct ringbuffer *rb)
{
    return rb->useful_size;
}
