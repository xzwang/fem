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
 * @Function	ringbuffer_init  - ������г�ʼ��
 *
 * @Param	rb - �������ָ��
 * @Param	pool - ��Ӧ�Ļ����
 * @Param	size - ����ش�С
 *****************************************************************/
void ringbuffer_init(struct ringbuffer *rb, void *pool, uint16 size);

/*****************************************************************
 * @Function	ringbuffer_getchar  - �ӽ��ն��ж�ȡһ���ֽ�
 *
 * @Param	rb - ���ն���ָ��
 * @Param	ch - �����ֽ�����
 *
 * @Returns	0-ʧ�� 1-�ɹ�
 *****************************************************************/
int ringbuffer_getchar(struct ringbuffer *rb, uint8 *ch);

/*****************************************************************
 * @Function	ringbuffer_putchar  - ���һ���ֽڵ����Ͷ���
 *
 * @Param	rb - ���Ͷ���ָ��
 * @Param	ch - �������
 *
 * @Returns	0-ʧ�� 1-�ɹ�
 *****************************************************************/
int ringbuffer_putchar(struct ringbuffer *rb, const uint8 ch);

/*****************************************************************
 * @Function	ringbuffer_get  - �ӽ��ն��л�ȡ������
 *
 * @Param	rb - �������ָ��
 * @Param	data - �û�����
 *
 * @Returns	���յ������ݳ���(0~max for buffer)
 *****************************************************************/
 uint16 ringbuffer_get(struct ringbuffer *rb, void *data);
 
/*****************************************************************
 * @Function	ringbuffer_put  - ��������������Ͷ���
 *
 * @Param	rb - ���Ͷ���ָ��
 * @Param	data - �û�����
 * @Param	len - �û����ݳ���
 *
 * @Returns	ʵ�ʷ��͵����ݳ���(0~len)
 *****************************************************************/
 uint16 ringbuffer_put(struct ringbuffer *rb, const void *data, uint16 len);
 
/*****************************************************************
 * @Function	ringbuffer_useful_size  - ��ȡ���л������ݳ���
 *
 * @Param	rb - �������ָ��
 *
 * @Returns	���������ݳ���(0~N)
 *****************************************************************/
uint16 ringbuffer_useful_size(struct ringbuffer *rb);
 
#endif /* _RING_BUFFER_H_ */

